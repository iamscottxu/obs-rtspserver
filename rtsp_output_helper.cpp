#include <vector>
#include <cstdio>
#include <obs-module.h>
#include <obs-frontend-api.h>
#include <util/config-file.h>
#include <util/dstr.h>
#include "rtsp_output_helper.h"
#include "helper.h"

using namespace std;
RtspOutputHelper::RtspOutputHelper(const string outputName)
	: RtspOutputHelper(obs_get_output_by_name(outputName.c_str()))
{
}

RtspOutputHelper::RtspOutputHelper(obs_output_t *obsOutput)
	: obsOutput(obsOutput)
{
}

RtspOutputHelper::~RtspOutputHelper()
{
	obs_output_release(obsOutput);
	obs_encoder_release(videoEncoder);
	for (const auto audioEncoder : audioEncoders)
		obs_encoder_release(audioEncoder);
}

RtspOutputHelper *RtspOutputHelper::CreateRtspOutput(obs_data_t *settings,
						     obs_data_t *hotkey)
{
	const auto rtspOutput = new RtspOutputHelper(
		obs_output_create("rtsp_output", obs_module_text("RtspOutput"),
				  settings, hotkey));
	rtspOutput->SignalConnect(
		"pre_start", OnPreStartSignal, rtspOutput);
	return rtspOutput;
}

void RtspOutputHelper::UpdateSettings(obs_data_t *settings) const
{
	obs_output_update(obsOutput, settings);
}

obs_data_t *RtspOutputHelper::GetSettings() const
{
	return obs_output_get_settings(obsOutput);
}

void RtspOutputHelper::UpdateEncoder()
{
	GetBaseConfig();
	CreateVideoEncoder();
	CreateAudioEncoder();
}

bool RtspOutputHelper::Start() const
{
	return obs_output_start(obsOutput);
}

void RtspOutputHelper::Stop() const
{
	obs_output_stop(obsOutput);
}

string RtspOutputHelper::GetLastError() const
{
	return obs_output_get_last_error(obsOutput);
}

obs_data_t *RtspOutputHelper::HotkeysSave() const
{
	return obs_hotkeys_save_output(obsOutput);
}

void RtspOutputHelper::SignalConnect(const char *signal,
				     signal_callback_t callback, void *data) const
{
	const auto handler = obs_output_get_signal_handler(obsOutput);
	signal_handler_connect(handler, signal, callback, data);
}

void RtspOutputHelper::SignalDisconnect(const char *signal,
					signal_callback_t callback, void *data) const
{
	const auto handler = obs_output_get_signal_handler(obsOutput);
	signal_handler_disconnect(handler, signal, callback, data);
}

string RtspOutputHelper::GetOutputName() const
{
	return string(obs_output_get_name(obsOutput));
}

uint64_t RtspOutputHelper::GetTotalBytes() const
{
	return obs_output_get_total_bytes(obsOutput);
}

int RtspOutputHelper::GetTotalFrames() const
{
	return obs_output_get_total_frames(obsOutput);
}

int RtspOutputHelper::GetFramesDropped() const
{
	return obs_output_get_frames_dropped(obsOutput);
}

bool RtspOutputHelper::IsActive() const
{
	return obs_output_active(obsOutput);
}

void RtspOutputHelper::CreateVideoEncoder()
{
	obs_encoder_t *encoder;
	if (outputSettings.adv_out)
		//encoder = obs_get_encoder_by_name("streaming_h264"); //OBS 27.2.4 Or Older
		encoder = obs_get_encoder_by_name("advanced_video_stream");
	else
		//encoder = obs_get_encoder_by_name("simple_h264_stream"); //OBS 27.2.4 Or Older
		encoder = obs_get_encoder_by_name("simple_video_stream");
	obs_encoder_release(videoEncoder);
	videoEncoder = obs_video_encoder_create(
		obs_encoder_get_id(encoder), "rtsp_output_video",
		obs_encoder_get_settings(encoder), nullptr);
	obs_encoder_release(encoder);
	if (outputSettings.adv_out)
		obs_encoder_set_scaled_size(videoEncoder,
					    outputSettings.rescale_cx,
					    outputSettings.rescale_cy);
	obs_encoder_set_video(videoEncoder, obs_get_video());
	{
		auto video = obs_output_video(obsOutput);
		if (video == nullptr)
			video = obs_get_video();
		obs_encoder_set_video(videoEncoder, video);
	}
	obs_output_set_video_encoder(obsOutput, videoEncoder);
}

void RtspOutputHelper::CreateAudioEncoder()
{
	obs_encoder_t *encoder;
	if (outputSettings.adv_out) {
		/*if ((encoder = obs_get_encoder_by_name("adv_stream_aac")) ==
		    nullptr)
			encoder = obs_get_encoder_by_name(
				"avc_aac_stream");*/ //OBS 26.0.2 Or Older
		/*if ((encoder = obs_get_encoder_by_name("adv_stream_audio")) ==
		    nullptr) //OBS 30.0.0 Or Older*/
		encoder = obs_get_encoder_by_name("adv_stream_aac");
	}
	else
		encoder = obs_get_encoder_by_name("simple_aac");

	for (const auto audioEncoder : audioEncoders)
		obs_encoder_release(audioEncoder);
	audioEncoders.clear();

	const auto config = rtsp_properties_open_config();

	auto tracks =
		static_cast<uint8_t>(config_get_uint(config, CONFIG_SECTIION, "AudioTracks"));
	{
		auto outputData = GetSettings();
		if (tracks == 0) {
			obs_data_set_bool(outputData, "output_audio", false);
			tracks = 0x1;
		} else obs_data_set_bool(outputData, "output_audio", true);
		UpdateSettings(outputData);
		obs_data_release(outputData);
	}
	auto trackIndex = 0;
	for (auto idx = 0; idx < OBS_OUTPUT_MULTI_TRACK; idx++) {
		if ((tracks & (1 << idx)) == 0) continue;
		auto audioEncoder = obs_audio_encoder_create(
			obs_encoder_get_id(encoder),
			string("rtsp_output_audio_track")
				.append(to_string(idx + 1))
				.c_str(),
			obs_encoder_get_settings(encoder), idx, nullptr);
		{
			auto audio = obs_output_audio(obsOutput);
			if (audio == nullptr)
				audio = obs_get_audio();
			obs_encoder_set_audio(audioEncoder, audio);
		}
		audioEncoders.push_back(audioEncoder);
		obs_output_set_audio_encoder(obsOutput, audioEncoder,
					     trackIndex++);
	}
	config_close(config);
	obs_encoder_release(encoder);
}

void RtspOutputHelper::GetBaseConfig()
{
	config_t *basicConfig = obs_frontend_get_profile_config();
	const char *mode = config_get_string(basicConfig, "Output", "Mode");
	outputSettings.adv_out = astrcmpi(mode, "Advanced") == 0;

	outputSettings.rescale_cx = 0;
	outputSettings.rescale_cy = 0;

	if (outputSettings.adv_out) {
		const bool rescale =
			config_get_bool(basicConfig, "AdvOut", "Rescale");

		if (const char *rescaleRes =
			config_get_string(basicConfig, "AdvOut", "RescaleRes"); rescale && rescaleRes && *rescaleRes) {
			if (sscanf(rescaleRes, "%ux%u",
				   &outputSettings.rescale_cx,
				   &outputSettings.rescale_cy) != 2) {
				outputSettings.rescale_cx = 0;
				outputSettings.rescale_cy = 0;
			}
		}
	}
}

void RtspOutputHelper::OnPreStartSignal(void *data, calldata_t *cd)
{
	UNUSED_PARAMETER(cd);
	const auto helper = static_cast<RtspOutputHelper *>(data);
	helper->UpdateEncoder();
}
