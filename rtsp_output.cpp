#include <thread>
#include <memory>
#include <map>
#include <iostream>
#include <obs-module.h>
#include <util/threading.h>
#include <xop/RtspServer.h>
#include "threadsafe_queue.h"
#include "rtsp_output.h"

#define USEC_IN_SEC 1000000

#define ERROR_BEGIN_DATA_CAPTURE 1
#define ERROR_INIT_ENCODERS 2
#define ERROR_START_RTSP_SERVER 3
#define ERROR_ENCODE OBS_OUTPUT_ENCODE_ERROR

#define AUDIO_TRACK_COUNT 6

struct queue_frame {
	struct xop::AVFrame av_frame;
	xop::MediaChannelId channe_id;
};

struct rtsp_out_data {
	obs_output_t *output = nullptr;

	uint16_t port = 0;

	volatile bool active;
	volatile bool stopping;
	uint64_t stop_ts;

	uint32_t num_clients = 0;
	std::map<size_t, uint32_t> audio_timestamp_clocks;
        std::map<size_t, xop::MediaChannelId> channel_ids;

	std::unique_ptr<xop::EventLoop> event_loop;
	std::shared_ptr<xop::RtspServer> server;
	xop::MediaSessionId session_id = NULL;
	std::unique_ptr<threadsafe_queue<queue_frame>> frame_queue;
	std::unique_ptr<std::thread> frame_push_thread;
};

static const char *rtsp_output_getname(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("RtspOutput");
}

static inline bool stopping(rtsp_out_data *out_data)
{
	return os_atomic_load_bool(&out_data->stopping);
}

static inline bool active(rtsp_out_data *out_data)
{
	return os_atomic_load_bool(&out_data->active);
}

static void rtsp_output_destroy(void *data)
{
	bfree(data);
}

static void rtsp_output_update(void *data, obs_data_t *settings);
static void *rtsp_output_create(obs_data_t *settings, obs_output_t *output)
{
	rtsp_out_data *data =
		(rtsp_out_data *)bzalloc(sizeof(struct rtsp_out_data));

	data->output = output;

	data->event_loop = std::make_unique<xop::EventLoop>();
        data->audio_timestamp_clocks = std::map<size_t, uint32_t>();
        data->channel_ids = std::map<size_t, xop::MediaChannelId>();
	data->server = xop::RtspServer::Create(data->event_loop.get());

        rtsp_output_update(data, settings);

	UNUSED_PARAMETER(settings);
	return data;
}

static void rtsp_push_frame(void *param);
static void set_output_error(rtsp_out_data *out_data, char code, ...)
{
	char *message;
	char *lookup_string;
	switch (code)
	{
	case ERROR_BEGIN_DATA_CAPTURE:
		message = "can't begin data capture";
		lookup_string = "ErrorBeginDataCapture";
		break;
	case ERROR_INIT_ENCODERS:
		message = "initialize encoders error";
		lookup_string = "ErrorInitEncoders";
		break;
	case ERROR_START_RTSP_SERVER:
		message = "starting RTSP server failed on port '%d'";
		lookup_string = "ErrorStartRtspServer";
		break;
	case ERROR_ENCODE:
		message = "encode error";
		lookup_string = "ErrorEncode";
		break;
	default:
		message = "unknown error";
		lookup_string = "ErrorUnknown";
		break;
	}

	{
		char buffer[500] = {0};
		va_list args;
		va_start(args, code);
#if defined(WIN32) || defined(_WIN32)
		vsprintf_s(buffer, obs_module_text(lookup_string), args);
#else
		vsnprintf(buffer, sizeof(buffer), obs_module_text(lookup_string),
			 args);
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


static bool rtsp_output_add_video_channel(void *data, xop::MediaSession *session)
{
        auto *out_data = (rtsp_out_data *)data;
        auto video_encoder = obs_output_get_video_encoder(out_data->output);
        if (video_encoder == nullptr) {
                set_output_error(out_data, ERROR_INIT_ENCODERS);
                return false;
        }
	auto video = obs_encoder_video(video_encoder);
        auto video_frame_rate = video_output_get_frame_rate(video);
        session->AddSource(xop::channel_0, xop::H264Source::CreateNew(
                (uint32_t)video_frame_rate));
	return true;
}

static bool rtsp_output_add_audio_channel(void *data, xop::MediaSession *session, size_t idx, xop::MediaChannelId channel_id)
{
        auto *out_data = (rtsp_out_data *)data;
        auto audio_encoder = obs_output_get_audio_encoder(out_data->output, idx);
	if (audio_encoder == nullptr) {
                set_output_error(out_data, ERROR_INIT_ENCODERS);
                return false;
	}
        auto audio = obs_encoder_audio(audio_encoder);
        auto audio_info = audio_output_get_info(audio);
        auto audio_channels = get_audio_channels(audio_info->speakers);
        auto audio_sample_rate = obs_encoder_get_sample_rate(audio_encoder);
        session->AddSource(channel_id,
                           xop::AACSource::CreateNew(audio_sample_rate,
                                                     audio_channels, false));
        out_data->audio_timestamp_clocks[idx] = audio_sample_rate;
        return true;
}

static bool rtsp_output_start(void *data)
{
	rtsp_out_data *out_data = (rtsp_out_data *)data;

	auto av_flags = out_data->channel_ids.size() > 0 ?  0 : OBS_OUTPUT_AUDIO;
	if (!obs_output_can_begin_data_capture(out_data->output, av_flags)) {
		set_output_error(out_data, ERROR_BEGIN_DATA_CAPTURE);
		return false;
	}

	if (!obs_output_initialize_encoders(out_data->output, av_flags)) {
		set_output_error(out_data, ERROR_INIT_ENCODERS);
		return false;
	}

	if (!out_data->server->Start("0.0.0.0", out_data->port) ||
	    !out_data->server->Start("::0", out_data->port)) {
		set_output_error(out_data, ERROR_START_RTSP_SERVER,
				 out_data->port);
		out_data->server->Stop();
		return false;
	}

	os_atomic_set_bool(&out_data->stopping, false);

        xop::MediaSession *session = xop::MediaSession::CreateNew("live", 4);

        out_data->audio_timestamp_clocks.clear();
        if (!rtsp_output_add_video_channel(data, session)) {
                return false;
        }
	for (auto iter : out_data->channel_ids) {
                if (!rtsp_output_add_audio_channel(data, session, iter.first, iter.second)) {
                        return false;
                }
	}

	out_data->frame_queue =
		std::make_unique<threadsafe_queue<queue_frame>>();

	session->SetNotifyCallback([out_data](xop::MediaSessionId session_id,
					      uint32_t num_clients) {
		if (num_clients > 0 && out_data->num_clients == 0) {
			obs_output_pause(out_data->output, false);
		}
		out_data->num_clients = num_clients;
		blog(LOG_INFO, "the number of rtsp clients: %d", num_clients);
	});

	out_data->session_id = out_data->server->AddSession(session);

	out_data->frame_push_thread =
		std::make_unique<std::thread>(rtsp_push_frame, out_data);

	os_atomic_set_bool(&out_data->active, true);
	obs_output_begin_data_capture(out_data->output, 0);

	blog(LOG_INFO, "starting rstp server on port '%d'", out_data->port);

	return true;
}

static void rtsp_output_stop(void *data, uint64_t ts)
{
	rtsp_out_data *out_data = (rtsp_out_data *)data;
	out_data->stop_ts = ts / 1000ULL;
	obs_output_pause(out_data->output, false);
	os_atomic_set_bool(&out_data->stopping, true);
}

static void rtsp_output_actual_stop(rtsp_out_data *out_data, int code)
{
	os_atomic_set_bool(&out_data->active, false);

	if (code) {
		set_output_error(out_data, code);
		obs_output_signal_stop(out_data->output, code);
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
		out_data->session_id = NULL;
	}
	out_data->server->Stop();
	out_data->num_clients = 0;

	if (out_data->frame_queue)
		out_data->frame_queue.reset();

	blog(LOG_INFO, "rstp server stopped");
}

static size_t get_video_header(obs_encoder_t *vencoder, uint8_t **header)
{
	uint8_t *extra_data;
	size_t extra_data_size;
	obs_encoder_get_extra_data(vencoder, &extra_data, &extra_data_size);
	*header = extra_data + 4;
	return extra_data_size - 4;
}

static uint32_t get_timestamp(uint64_t timestamp_clock,
			      struct encoder_packet *packet)
{
	// Convert the incoming dts time to the correct clock time for the timestamp.
	// We use a int64 to ensure the roll over is handled correctly.
	// We do the [USEC_IN_SEC / 2] trick to make sure the result of the division rounds to the nearest int.
	uint64_t timestamp = packet->dts_usec * timestamp_clock;
	return (uint32_t)((timestamp + USEC_IN_SEC / 2) / USEC_IN_SEC);
}

static void rtsp_push_frame(void *param)
{
	rtsp_out_data *out_data = (rtsp_out_data *)param;

	blog(LOG_INFO, "starting rtsp frame push thread");
	while (true) {
		std::shared_ptr<queue_frame> queue_frame =
			out_data->frame_queue->wait_and_pop();
		if (queue_frame == nullptr)
			break;
		out_data->server->PushFrame(out_data->session_id,
					    queue_frame->channe_id,
					    queue_frame->av_frame);
	}
	blog(LOG_INFO, "rtsp frame push thread stopped");
}

static void rtsp_output_video(void *param, struct encoder_packet *packet)
{
	rtsp_out_data *out_data = (rtsp_out_data *)param;
	xop::AVFrame videoFrame = {0};
	videoFrame.timestamp = get_timestamp(90000, packet);
	if (packet->keyframe) {
		videoFrame.type = xop::VIDEO_FRAME_I;
		uint8_t *header;
		size_t header_size = get_video_header(
			obs_output_get_video_encoder(out_data->output),
			&header);
		videoFrame.size = packet->size + header_size;
		videoFrame.buffer.reset(new uint8_t[videoFrame.size]);
		memcpy(videoFrame.buffer.get(), header, header_size);
		memcpy(videoFrame.buffer.get() + header_size, packet->data,
		       packet->size);
	} else {
		if (packet->pts == packet->dts)
			videoFrame.type = xop::VIDEO_FRAME_I;
		else if (packet->pts > packet->dts)
			videoFrame.type = xop::VIDEO_FRAME_P;
		else
			videoFrame.type = xop::VIDEO_FRAME_B;
		videoFrame.size = packet->size;
		videoFrame.buffer.reset(new uint8_t[videoFrame.size]);
		memcpy(videoFrame.buffer.get(), packet->data, packet->size);
	}

	struct queue_frame queue_frame;
	queue_frame.av_frame = videoFrame;
	queue_frame.channe_id = xop::channel_0;
	out_data->frame_queue->push(queue_frame);
}

static void rtsp_output_audio(void *param, struct encoder_packet *packet)
{
	rtsp_out_data *out_data = (rtsp_out_data *)param;
	xop::AVFrame audioFrame = {0};
	audioFrame.type = xop::AUDIO_FRAME;
	audioFrame.size = packet->size;
	audioFrame.timestamp =
		get_timestamp(out_data->audio_timestamp_clocks[packet->track_idx], packet);
	audioFrame.buffer.reset(new uint8_t[audioFrame.size]);
	memcpy(audioFrame.buffer.get(), packet->data, packet->size);

	struct queue_frame queue_frame;
	queue_frame.av_frame = audioFrame;
	queue_frame.channe_id = out_data->channel_ids[packet->track_idx];
	out_data->frame_queue->push(queue_frame);
}

static void rtsp_output_data(void *data, struct encoder_packet *packet)
{
	rtsp_out_data *out_data = (rtsp_out_data *)data;

	if (!active(out_data))
		return;

	if (!packet) {
		rtsp_output_actual_stop(out_data, OBS_OUTPUT_ENCODE_ERROR);
		return;
	}

	if (stopping(out_data) &&
	    packet->sys_dts_usec >= (int64_t)out_data->stop_ts) {
		rtsp_output_actual_stop(out_data, OBS_OUTPUT_SUCCESS);
		return;
	}

	if (out_data->num_clients > 0) {
		if (packet->type == OBS_ENCODER_VIDEO)
			rtsp_output_video(data, packet);
		else if (packet->type == OBS_ENCODER_AUDIO)
			rtsp_output_audio(data, packet);
	} else if (!stopping(out_data)) {
		obs_output_pause(out_data->output, true);
	}
}

static void rtsp_output_defaults(obs_data_t *defaults)
{
	obs_data_set_default_int(defaults, "port", 554);
        for (size_t index = 0; index < AUDIO_TRACK_COUNT; index++)
        {
                auto name = string("audio_track").append(to_string(index + 1));
                obs_data_set_default_bool(defaults, name.c_str(), index == 0);
        }
}

static void rtsp_output_update(void *data, obs_data_t *settings)
{
	rtsp_out_data *out_data = (rtsp_out_data *)data;
	out_data->port = obs_data_get_int(settings, "port");

	out_data->channel_ids.clear();
	size_t channel_index = 1;
        for (size_t index = 0; index < AUDIO_TRACK_COUNT; index++)
        {
                auto name = string("audio_track").append(to_string(index + 1));
                if (obs_data_get_bool(settings, name.c_str()))
			out_data->channel_ids[index] = (xop::MediaChannelId)channel_index++;
	}
}

obs_properties_t *rtsp_output_properties(void *data)
{
	UNUSED_PARAMETER(data);

	obs_properties_t *props = obs_properties_create();
	obs_properties_set_flags(props, OBS_PROPERTIES_DEFER_UPDATE);

	obs_properties_add_int(props, "port",
			       obs_module_text("RtspOutput.Port"), 1, 65535, 1);

        obs_properties_t *audio_tracks_props = obs_properties_create();
	for (size_t index = 0; index < AUDIO_TRACK_COUNT; index++)
        {
		auto name = string("audio_track").append(to_string(index + 1));
                auto lookup_string = string("RtspOutput.AudioTrack").append(to_string(index + 1));
                obs_properties_add_bool(audio_tracks_props, name.c_str(),
                                        obs_module_text(lookup_string.c_str()));
	}
	obs_properties_add_group(props, "audio_tracks",
				 obs_module_text("RtspOutput.AudioTracks"), OBS_GROUP_NORMAL, audio_tracks_props);
	return props;
}

void rtsp_output_register()
{
	struct obs_output_info output_info = {};
	output_info.id = "rtsp_output";
	output_info.flags = OBS_OUTPUT_AV | OBS_OUTPUT_ENCODED |
			    OBS_OUTPUT_CAN_PAUSE | OBS_OUTPUT_MULTI_TRACK;
	output_info.encoded_video_codecs = "h264";
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

	obs_register_output(&output_info);
}
