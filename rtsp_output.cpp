#include <thread>
#include <memory>
#include <array>
#include <string>
#include <obs-module.h>
#include <obs-encoder.h>
#include <util/threading.h>
#include <xop/RtspServer.h>
#include <xop/H264Source.h>
#include <xop/H265Source.h>
#include <xop/AACSource.h>
#include "threadsafe_queue.h"
#include "rtsp_output.h"
#include "helper.h"

#define USEC_IN_SEC 1000000

#define ERROR_BEGIN_DATA_CAPTURE 1
#define ERROR_INIT_ENCODERS 2
#define ERROR_START_RTSP_SERVER 3
#define ERROR_START_MULTICAST 4
#define ERROR_ENCODE OBS_OUTPUT_ENCODE_ERROR

#define OBS_RTSPSERVER_QUEUE_SIZE_LIMIT 2

struct queue_frame {
	queue_frame(size_t size = 0) : av_frame(size), channe_id(xop::MediaChannelId::channel_0) {}
	struct xop::AVFrame av_frame;
	xop::MediaChannelId channe_id;
};

struct rtsp_out_data {
	obs_output_t *output = nullptr;

	volatile bool active;
	volatile bool starting;
	volatile bool stopping;
	uint64_t stop_ts;

	uint32_t num_clients = 0;
	std::array<uint32_t, OBS_OUTPUT_MULTI_TRACK> audio_timestamp_clocks;
	std::array<bool, OBS_OUTPUT_MULTI_TRACK> enabled_channels;
	std::array<xop::MediaChannelId, OBS_OUTPUT_MULTI_TRACK> channel_ids;
	uint64_t total_bytes_sent = 0;
	uint32_t enabled_channels_count = 0;

	std::unique_ptr<xop::EventLoop> event_loop;
	std::shared_ptr<xop::RtspServer> server;
	xop::MediaSessionId session_id = 0;
	std::unique_ptr<threadsafe_queue<queue_frame>> frame_queue;
	std::unique_ptr<std::thread> frame_push_thread;

	obs_hotkey_pair_id start_stop_hotkey;
};

static const char *rtsp_output_getname(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("RtspOutput");
}

static inline bool active(rtsp_out_data *out_data)
{
	return os_atomic_load_bool(&out_data->active);
}

static inline bool starting(rtsp_out_data *out_data)
{
	return os_atomic_load_bool(&out_data->starting);
}

static inline bool stopping(rtsp_out_data *out_data)
{
	return os_atomic_load_bool(&out_data->stopping);
}

static void add_prestart_signal(rtsp_out_data *out_data)
{
	auto handler = obs_output_get_signal_handler(out_data->output);
	signal_handler_add(handler, "void pre_start()");
}

static void send_prestart_signal(rtsp_out_data *out_data)
{
	auto handler = obs_output_get_signal_handler(out_data->output);
	signal_handler_signal(handler, "pre_start", nullptr);
}

static bool rtsp_output_start_hotkey(void *data, const obs_hotkey_pair_id id,
                                     obs_hotkey_t *hotkey, const bool pressed)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(hotkey);

	const auto out_data = static_cast<rtsp_out_data *>(data);

	if (!pressed)
		return false;
	if (stopping(out_data) || starting(out_data) || active(out_data))
		return false;

	return obs_output_start(out_data->output);
}

static bool rtsp_output_stop_hotkey(void *data, const obs_hotkey_pair_id id,
                                    obs_hotkey_t *hotkey, const bool pressed)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(hotkey);

	auto *out_data = static_cast<rtsp_out_data *>(data);

	if (!pressed)
		return false;
	if (stopping(out_data) || starting(out_data) || !active(out_data))
		return false;

	obs_output_stop(out_data->output);

	return true;
}

static void rtsp_output_destroy(void *data)
{
	//rtsp_out_data *out_data = (rtsp_out_data *)data;
	bfree(data);
}

static void rtsp_output_update(void *data, obs_data_t *settings);
static void *rtsp_output_create(obs_data_t *settings, obs_output_t *output)
{
	auto data =
		static_cast<rtsp_out_data *>(bzalloc(sizeof(struct rtsp_out_data)));

	data->output = output;

	data->event_loop = std::make_unique<xop::EventLoop>();
	data->server = xop::RtspServer::Create(data->event_loop.get());

	add_prestart_signal(data);

	data->start_stop_hotkey = obs_hotkey_pair_register_output(
		output, "RtspOutput.Start",
		obs_module_text("RtspOutput.Hotkey.StartOutput"),
		"RtspOutput.Stop",
		obs_module_text("RtspOutput.Hotkey.StopOutput"),
		rtsp_output_start_hotkey, rtsp_output_stop_hotkey, data, data);

	UNUSED_PARAMETER(settings);
	return data;
}

static void rtsp_push_frame(void *param);
static void set_output_error(const rtsp_out_data *out_data, int code, ...)
{
	const char *message;
	const char *lookup_string;
	switch (code) {
	case ERROR_BEGIN_DATA_CAPTURE:
		message = "can't begin data capture";
		lookup_string = "RtspOutput.Error.BeginDataCapture";
		break;
	case ERROR_INIT_ENCODERS:
		message = "initialize encoders error";
		lookup_string = "RtspOutput.Error.InitEncoders";
		break;
	case ERROR_START_RTSP_SERVER:
		message = "starting RTSP server failed on port '%d'";
		lookup_string = "RtspOutput.Error.StartRtspServer";
		break;
	case ERROR_START_MULTICAST:
		message = "starting multicast failed";
		lookup_string = "RtspOutput.Error.StartMulticast";
		break;
	case ERROR_ENCODE:
		message = "encode error";
		lookup_string = "RtspOutput.Error.Encode";
		break;
	default:
		message = "unknown error";
		lookup_string = "RtspOutput.Error.Unknown";
		break;
	}

	{
		char buffer[500] = {0};
		va_list args;
		va_start(args, code);
#if defined(WIN32) || defined(_WIN32)
		vsprintf_s(buffer, obs_module_text(lookup_string), args);
#else
		vsnprintf(buffer, sizeof(buffer),
			  obs_module_text(lookup_string), args);
#endif
		va_end(args);
		obs_output_set_last_error(out_data->output, buffer);
	}

	{
		va_list args;
		va_start(args, code);
		blogva(LOG_WARNING, message, args);
		va_end(args);
	}
}

static bool rtsp_output_add_video_channel(void *data,
					  xop::MediaSession *session)
{
	auto *out_data = static_cast<rtsp_out_data *>(data);
	const auto video_encoder =
		obs_output_get_video_encoder(out_data->output);
	if (video_encoder == nullptr)
		return false;
	const auto video = obs_encoder_video(video_encoder);
	const auto video_frame_rate = video_output_get_frame_rate(video);
	vector<uint8_t> extra_data;
	{
		uint8_t *p_extra_data = nullptr;
		size_t extra_data_size = 0;
		if (!obs_encoder_get_extra_data(video_encoder, &p_extra_data,
						&extra_data_size))
			extra_data_size = 0;
		extra_data = vector<uint8_t>(p_extra_data,
					     p_extra_data + extra_data_size);
	}
	switch (get_encoder_codec(video_encoder)) {
	case encoder_codec::H264: {
		session->AddSource(
			xop::MediaChannelId::channel_0,
			xop::H264Source::CreateNew(
				extra_data,
				static_cast<uint32_t>(video_frame_rate)));
	}
		break;
	case encoder_codec::HEVC: {
		session->AddSource(
			xop::MediaChannelId::channel_0,
			xop::H265Source::CreateNew(
				extra_data,
				vector<uint8_t>(),
				static_cast<uint32_t>(video_frame_rate)));
	}
		break;
	default:
		break;
	}
	return true;
}

static bool rtsp_output_add_audio_channel(void *data,
					  xop::MediaSession *session,
					  const size_t idx,
					  const xop::MediaChannelId channel_id)
{
	auto *out_data = static_cast<rtsp_out_data *>(data);
	const auto audio_encoder =
		obs_output_get_audio_encoder(out_data->output, idx);
	if (audio_encoder == nullptr) 
		return false;
	const auto audio = obs_encoder_audio(audio_encoder);
	const auto audio_channels = audio_output_get_channels(audio);
	const auto audio_sample_rate =
		obs_encoder_get_sample_rate(audio_encoder);
	uint8_t *extra_data = nullptr;
	size_t extra_data_size = 0;
	if (obs_encoder_get_extra_data(audio_encoder, &extra_data,
				       &extra_data_size)) {
	}
	session->AddSource(
		channel_id,
		xop::AACSource::CreateNew(audio_sample_rate,
					  static_cast<uint8_t>(audio_channels),
					  false));
	out_data->audio_timestamp_clocks[idx] = audio_sample_rate;
	return true;
}

static bool rtsp_output_start(void *data)
{
	auto out_data = static_cast<rtsp_out_data *>(data);

	if (starting(out_data) || stopping(out_data))
		return false;

	send_prestart_signal(out_data);

	const auto settings = obs_output_get_settings(out_data->output);
	rtsp_output_update(data, settings);
	const auto port =
		static_cast<uint16_t>(obs_data_get_int(settings, "port"));

	uint32_t enabled_channels_count = 0;
	for (size_t index = 0; index < out_data->enabled_channels.size();
	     index++) {
		if (obs_output_get_audio_encoder(out_data->output, index) ==
		    nullptr) {
			out_data->enabled_channels[index] = false;
			continue;
		}
		out_data->enabled_channels[index] = true;
		out_data->channel_ids[index] = static_cast<xop::MediaChannelId>(
			++enabled_channels_count);
	}
	out_data->enabled_channels_count = enabled_channels_count;

	const uint32_t av_flags = enabled_channels_count > 0 ? 0 : OBS_OUTPUT_VIDEO;
	if (!obs_output_can_begin_data_capture(out_data->output, av_flags)) {
		set_output_error(out_data, ERROR_BEGIN_DATA_CAPTURE);
		return false;
	}

	if (!obs_output_initialize_encoders(out_data->output, av_flags)) {
		set_output_error(out_data, ERROR_INIT_ENCODERS);
		return false;
	}

	if (!out_data->server->Start("0.0.0.0", port) ||
	    !out_data->server->Start("::0", port)) {
		set_output_error(out_data, ERROR_START_RTSP_SERVER, port);
		out_data->server->Stop();
		return false;
	}

	obs_output_begin_data_capture(out_data->output, av_flags);

	os_atomic_set_bool(&out_data->starting, true);

	return true;
}

static void rtsp_output_actual_stop(rtsp_out_data *out_data, const int code);
static void rtsp_output_rtsp_start(void *data)
{
	const auto out_data = static_cast<rtsp_out_data *>(data);

	const auto settings = obs_output_get_settings(out_data->output);
	const auto port =
		static_cast<uint16_t>(obs_data_get_int(settings, "port"));
	const auto url_suffix = obs_data_get_string(settings, "url_suffix");

	xop::MediaSession *session = xop::MediaSession::CreateNew(
		url_suffix, out_data->enabled_channels_count + 1);

	if (!rtsp_output_add_video_channel(data, session)) {
		rtsp_output_actual_stop(out_data, ERROR_INIT_ENCODERS);
		return;
	}

	for (size_t index = 0; index < out_data->enabled_channels.size();
	     index++) {
		if (!out_data->enabled_channels[index])
			continue;
		if (!rtsp_output_add_audio_channel(
			    data, session, index,
			    out_data->channel_ids[index])) {
			rtsp_output_actual_stop(out_data, ERROR_INIT_ENCODERS);
			return;
		}
	}

	out_data->frame_queue = std::make_unique<threadsafe_queue<queue_frame>>(
		OBS_RTSPSERVER_QUEUE_SIZE_LIMIT);

	session->AddNotifyConnectedCallback(
		[](const xop::MediaSessionId session_id,
		   const std::string &peer_ip, const uint16_t peer_port) {
			blog(LOG_INFO, "Rtsp client %d(%s:%d) is connected.",
			     session_id, peer_ip.c_str(), peer_port);
		});

	session->AddNotifyDisconnectedCallback(
		[](const xop::MediaSessionId session_id,
		   const std::string &peer_ip, const uint16_t peer_port) {
			blog(LOG_INFO, "Rtsp client %d(%s:%d) is disconnected.",
			     session_id, peer_ip.c_str(), peer_port);
		});

	out_data->session_id = out_data->server->AddSession(session);

	if (auto multicast = obs_data_get_bool(settings, "multicast")) {
		if (multicast = session->StartMulticast(); multicast) {
			blog(LOG_INFO,
			     "------------------------------------------------");
			blog(LOG_INFO, "rtsp multicast info:");
			blog(LOG_INFO, "\tipv6 address:        \t%s",
			     session->GetMulticastIp(true).c_str());
			blog(LOG_INFO, "\tipv4 address:        \t%s",
			     session->GetMulticastIp(false).c_str());
			for (uint32_t i = 0;
			     i < out_data->enabled_channels_count + 1; i++) {
				blog(LOG_INFO, "\tchannel %d port: \t%d", i,
				     session->GetMulticastPort(
					     static_cast<xop::MediaChannelId>(
						     i)));
			}
			blog(LOG_INFO,
			     "------------------------------------------------");
		} else {
			rtsp_output_actual_stop(out_data, ERROR_START_MULTICAST);
			return;
		}
	}

	out_data->total_bytes_sent = 0;

	out_data->frame_push_thread =
		std::make_unique<std::thread>(rtsp_push_frame, out_data);

	os_atomic_set_bool(&out_data->active, true);
	os_atomic_set_bool(&out_data->starting, false);

	blog(LOG_INFO, "starting rstp server on port '%d'", port);
}

static void rtsp_output_stop(void *data, uint64_t ts)
{
	auto *out_data = static_cast<rtsp_out_data *>(data);

	if (starting(out_data) || stopping(out_data))
		return;

	out_data->stop_ts = ts / 1000ULL;
	//obs_output_pause(out_data->output, false);
	os_atomic_set_bool(&out_data->stopping, true);
}

static void rtsp_output_actual_stop(rtsp_out_data *out_data, const int code)
{
	os_atomic_set_bool(&out_data->active, false);

	if (code < 0) {
		set_output_error(out_data, code);
		obs_output_signal_stop(out_data->output, code);
	} else if (code > 0) {
		obs_output_end_data_capture(out_data->output);
		set_output_error(out_data, code);
		obs_output_signal_stop(out_data->output, OBS_OUTPUT_ERROR);
	} else {
		obs_output_end_data_capture(out_data->output);
	}

	if (out_data->frame_queue)
		out_data->frame_queue->termination();

	if (out_data->frame_push_thread) {
		out_data->frame_push_thread->join();
		out_data->frame_push_thread.reset();
	}

	if (out_data->session_id) {
		out_data->server->RemoveSession(out_data->session_id);
		out_data->session_id = 0;
	}
	out_data->server->Stop();
	out_data->num_clients = 0;

	if (out_data->frame_queue)
		out_data->frame_queue.reset();

	os_atomic_set_bool(&out_data->starting, false);
	os_atomic_set_bool(&out_data->stopping, false);

	blog(LOG_INFO, "rstp server stopped");
}

static uint32_t get_timestamp(uint64_t timestamp_clock,
			      struct encoder_packet *packet)
{
	// Convert the incoming dts time to the correct clock time for the timestamp.
	// We use a int64 to ensure the roll over is handled correctly.
	// We do the [USEC_IN_SEC / 2] trick to make sure the result of the division rounds to the nearest int.
	const uint64_t timestamp = packet->dts_usec * timestamp_clock;
	return static_cast<uint32_t>((timestamp + USEC_IN_SEC / 2) /
				     USEC_IN_SEC);
}

static void rtsp_push_frame(void *param)
{
	auto *out_data = static_cast<rtsp_out_data *>(param);

	blog(LOG_INFO, "starting rtsp frame push thread");
	while (true) {
		std::shared_ptr<queue_frame> queue_frame =
			out_data->frame_queue->wait_and_pop();
		if (queue_frame == nullptr)
			break;
		out_data->total_bytes_sent += queue_frame->av_frame.size;
		out_data->server->PushFrame(out_data->session_id,
					    queue_frame->channe_id,
					    queue_frame->av_frame);
	}
	blog(LOG_INFO, "rtsp frame push thread stopped");
}

static void rtsp_output_video(void *param, struct encoder_packet *packet)
{
	const auto *out_data = static_cast<rtsp_out_data *>(param);

	struct queue_frame queue_frame(packet->size);
	xop::AVFrame *frame = &queue_frame.av_frame;
	queue_frame.channe_id = xop::MediaChannelId::channel_0;

	frame->timestamp = get_timestamp(90000, packet);

	memcpy(frame->buffer.get(), packet->data, packet->size);

	out_data->frame_queue->push(queue_frame);
}

static void rtsp_output_audio(void *param, struct encoder_packet *packet)
{
	const auto *out_data = static_cast<rtsp_out_data *>(param);

	queue_frame queue_frame(packet->size);
	xop::AVFrame *frame = &queue_frame.av_frame;
	queue_frame.channe_id = out_data->channel_ids[packet->track_idx];

	frame->timestamp = get_timestamp(
		out_data->audio_timestamp_clocks[packet->track_idx], packet);

	memcpy(frame->buffer.get(), packet->data, packet->size);

	out_data->frame_queue->push(queue_frame);
}

static void rtsp_output_data(void *data, struct encoder_packet *packet)
{
	auto *out_data = static_cast<rtsp_out_data *>(data);

	if (!active(out_data) && !starting(out_data))
		return;

	if (starting(out_data)) {
		rtsp_output_rtsp_start(out_data);
		return;
	}

	if (!packet) {
		rtsp_output_actual_stop(out_data, OBS_OUTPUT_ENCODE_ERROR);
		return;
	}

	if (stopping(out_data) &&
	    packet->sys_dts_usec >= static_cast<int64_t>(out_data->stop_ts)) {
		rtsp_output_actual_stop(out_data, OBS_OUTPUT_SUCCESS);
		return;
	}

	//if (out_data->num_clients > 0) {
	if (packet->type == OBS_ENCODER_VIDEO)
		rtsp_output_video(data, packet);
	else if (packet->type == OBS_ENCODER_AUDIO)
		rtsp_output_audio(data, packet);
	//} else if (!stopping(out_data)) {
	//obs_output_pause(out_data->output, true);
	//}
}

static void rtsp_output_defaults(obs_data_t *defaults)
{
	obs_data_set_default_bool(defaults, "multicast", false);
#if defined(__APPLE__) || defined(__MACH__)
	// On osx the application will run using a non-priviliged user.
	// Opening ports below 1024 is not possible.
	obs_data_set_default_int(defaults, "port", 8554);
#else
	obs_data_set_default_int(defaults, "port", 554);
#endif
	obs_data_set_default_string(defaults, "url_suffix", "live");
	obs_data_set_default_bool(defaults, "authentication", false);
	obs_data_set_default_string(defaults, "authentication_realm", "");
	obs_data_set_default_string(defaults, "authentication_username", "");
	obs_data_set_default_string(defaults, "authentication_password", "");
}

static void rtsp_output_update(void *data, obs_data_t *settings)
{
	const auto *out_data = static_cast<rtsp_out_data *>(data);
	const auto auth_enabled = obs_data_get_bool(settings, "authentication");
	const auto auth_realm =
		obs_data_get_string(settings, "authentication_realm");
	const auto auth_username =
		obs_data_get_string(settings, "authentication_username");
	const auto auth_password =
		obs_data_get_string(settings, "authentication_password");

	if (auth_enabled && auth_realm && *auth_realm != '\0' &&
	    auth_username && *auth_username != '\0')
		out_data->server->SetAuthConfig(auth_realm, auth_username,
						auth_password);
	else {
		obs_data_set_bool(settings, "authentication", false);
		out_data->server->SetAuthConfig("", "", "");
	}
}

static obs_properties_t *rtsp_output_properties(void *data)
{
	UNUSED_PARAMETER(data);

	obs_properties_t *props = obs_properties_create();
	obs_properties_set_flags(props, OBS_PROPERTIES_DEFER_UPDATE);

	obs_properties_add_bool(
		props, "multicast",
		obs_module_text("RtspOutput.Properties.Multicast"));
	obs_properties_add_int(props, "port",
			       obs_module_text("RtspOutput.Properties.Port"), 1,
			       65535, 1);

	obs_properties_add_text(
		props, "url_suffix",
		obs_module_text("RtspOutput.Properties.UrlSuffix"),
		OBS_TEXT_DEFAULT);

	obs_properties_t *auth_group = obs_properties_create();
	obs_properties_add_text(
		auth_group, "authentication_realm",
		obs_module_text("RtspOutput.Properties.Authentication.Realm"),
		OBS_TEXT_DEFAULT);
	obs_properties_add_text(
		auth_group, "authentication_username",
		obs_module_text(
			"RtspOutput.Properties.Authentication.Username"),
		OBS_TEXT_DEFAULT);
	obs_properties_add_text(
		auth_group, "authentication_password",
		obs_module_text(
			"RtspOutput.Properties.Authentication.Password"),
		OBS_TEXT_PASSWORD);
	obs_properties_add_group(
		props, "authentication",
		obs_module_text("RtspOutput.Properties.Authentication"),
		OBS_GROUP_CHECKABLE, auth_group);
	return props;
}

static uint64_t rtsp_output_total_bytes_sent(void *data)
{
	const auto *out_data = static_cast<rtsp_out_data *>(data);
	return out_data->total_bytes_sent;
}

static int rtsp_output_get_dropped_frames(void *data)
{
	const auto *out_data = static_cast<rtsp_out_data *>(data);
	if (!active(out_data) || out_data->frame_queue == nullptr) {
		return 0;
	}
	auto dropped_count = out_data->frame_queue->dropped_count();
	while (dropped_count > INT32_MAX)
		dropped_count -= INT32_MAX;
	return static_cast<int>(dropped_count);
}

void rtsp_output_register()
{
	struct obs_output_info output_info = {};
	output_info.id = "rtsp_output";
	output_info.flags = OBS_OUTPUT_AV | OBS_OUTPUT_ENCODED |
			    OBS_OUTPUT_MULTI_TRACK;
	output_info.encoded_video_codecs = "h264;hevc";
	output_info.encoded_audio_codecs = "aac";
	output_info.get_name = rtsp_output_getname;
	output_info.create = rtsp_output_create;
	output_info.destroy = rtsp_output_destroy;
	output_info.start = rtsp_output_start;
	output_info.stop = rtsp_output_stop;
	output_info.encoded_packet = rtsp_output_data;
	output_info.get_defaults = rtsp_output_defaults;
	output_info.update = rtsp_output_update;
	output_info.get_properties = rtsp_output_properties;
	output_info.get_total_bytes = rtsp_output_total_bytes_sent;
	output_info.get_dropped_frames = rtsp_output_get_dropped_frames;

	obs_register_output(&output_info);
}
