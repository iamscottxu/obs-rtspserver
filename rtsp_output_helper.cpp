#include <stdio.h>
#include <obs-module.h>
#include <obs-frontend-api.h>
#include <util/config-file.h>
#include <util/dstr.h>
#include "rtsp_output_helper.h"
#include "rtsp_output.h"

using namespace std;
RtspOutputHelper::RtspOutputHelper(string outputName)
	: RtspOutputHelper(obs_get_output_by_name(outputName.c_str()))
{

}

RtspOutputHelper::RtspOutputHelper(obs_output_t *obsOutput)
{
	this->obsOutput = obsOutput;
}

RtspOutputHelper::~RtspOutputHelper()
{
	obs_output_release(obsOutput);
	obs_encoder_release(videoEncoder);
	obs_encoder_release(audioEncoder);
}

RtspOutputHelper *RtspOutputHelper::CreateRtspOutput(obs_data_t *settings, obs_data_t *hotkey)
{
	auto rtspOutput =
		new RtspOutputHelper(obs_output_create("rtsp_output",
			obs_module_text("RtspOutput"),settings, hotkey));
	rtspOutput->audioEncoder = nullptr;
	rtspOutput->videoEncoder = nullptr;
	rtspOutput->SignalConnect("pre_start", RtspOutputHelper::OnPreStartSignal, rtspOutput);
	return rtspOutput;
}

void RtspOutputHelper::UpdateSettings(obs_data_t *settings)
{
	obs_output_update(obsOutput, settings);
}

obs_data_t *RtspOutputHelper::GetSettings()
{
	return obs_output_get_settings(obsOutput);
}

void RtspOutputHelper::UpdateEncoder()
{
	GetBaseConfig();
	CreateVideoEncoder();
	CreateAudioEncoder();
}

bool RtspOutputHelper::Start()
{
	return obs_output_start(obsOutput);
}

void RtspOutputHelper::Stop()
{
	obs_output_stop(obsOutput);
}

string RtspOutputHelper::GetLastError()
{
	return string(obs_output_get_last_error(obsOutput));
}

obs_data_t *RtspOutputHelper::HotkeysSave()
{
	return obs_hotkeys_save_output(obsOutput);
}

void RtspOutputHelper::SignalConnect(const char *signal, signal_callback_t callback,
				 void *data)
{
	auto handler = obs_output_get_signal_handler(obsOutput);
	signal_handler_connect(handler, signal, callback, data);
}

void RtspOutputHelper::SignalDisconnect(const char *signal,
				    signal_callback_t callback, void *data)
{
	auto handler = obs_output_get_signal_handler(obsOutput);
	signal_handler_disconnect(handler, signal, callback, data);
}

string RtspOutputHelper::GetOutputName()
{
	return string(obs_output_get_name(obsOutput));
}

uint64_t RtspOutputHelper::GetTotalBytes()
{
	return obs_output_get_total_bytes(obsOutput);
}

bool RtspOutputHelper::IsActive()
{
	return obs_output_active(obsOutput);
}

void RtspOutputHelper::CreateVideoEncoder()
{
	obs_encoder_t *encoder = nullptr;
	if (outputSettings.adv_out)
		encoder = obs_get_encoder_by_name("streaming_h264");
	else
		encoder = obs_get_encoder_by_name("simple_h264_stream");
	obs_encoder_release(videoEncoder);
	videoEncoder = obs_video_encoder_create(
		obs_encoder_get_id(encoder), "rtsp_output_video",
		obs_encoder_get_settings(encoder), NULL);
	obs_encoder_release(encoder);
	if (outputSettings.adv_out)
		obs_encoder_set_scaled_size(videoEncoder,
					    outputSettings.rescale_cx,
					    outputSettings.rescale_cy);
	obs_encoder_set_video(videoEncoder, obs_output_video(obsOutput));
	obs_output_set_video_encoder(obsOutput, videoEncoder);
}

void RtspOutputHelper::CreateAudioEncoder()
{
	obs_encoder_t *encoder = nullptr;
	if (outputSettings.adv_out) {
		if ((encoder =
			     obs_get_encoder_by_name("adv_stream_aac")) == NULL)
			encoder = obs_get_encoder_by_name(
				"avc_aac_stream"); //OBS 26.0.2 Or Older
	}
	else
		encoder = obs_get_encoder_by_name("simple_aac");
	obs_encoder_release(audioEncoder);
	audioEncoder = obs_audio_encoder_create(
		obs_encoder_get_id(encoder), "rtsp_output_audio",
		obs_encoder_get_settings(encoder), 0 , NULL);
	obs_encoder_release(encoder);
	obs_encoder_set_audio(audioEncoder, obs_output_audio(obsOutput));
	obs_output_set_audio_encoder(obsOutput, audioEncoder, 0);
}

void RtspOutputHelper::GetBaseConfig()
{
	config_t *basicConfig = obs_frontend_get_profile_config();
	const char *mode = config_get_string(basicConfig, "Output", "Mode");
	outputSettings.adv_out = astrcmpi(mode, "Advanced") == 0;

	outputSettings.rescale_cx = 0;
	outputSettings.rescale_cy = 0;

	if (outputSettings.adv_out) {
		bool rescale =
			config_get_bool(basicConfig, "AdvOut", "Rescale");
		const char *rescaleRes =
			config_get_string(basicConfig, "AdvOut", "RescaleRes");

		if (rescale && rescaleRes && *rescaleRes) {
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
	auto helper = (RtspOutputHelper *)data;
	helper->UpdateEncoder();
}
