#include <stdio.h>
#include <obs-module.h>
#include <obs-frontend-api.h>
#include <util/config-file.h>
#include <util/dstr.h>
#include "my_rtsp_output.h"
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
	releaseEncoder(&videoEncoder);
	releaseEncoder(&audioEncoder);
}

RtspOutputHelper *RtspOutputHelper::CreateRtspOutput(obs_data_t* settings)
{
	auto rtspOutput =
		new RtspOutputHelper(obs_output_create("rtsp_output", "RtspOutput",
						   settings, NULL));
	return rtspOutput;
}

void RtspOutputHelper::UpdateSettings(obs_data_t *settings)
{
	obs_output_update(obsOutput, settings);
}

void RtspOutputHelper::UpdateEncoder()
{
	GetBaseConfig();
	CreateVideoEncoder();
	CreateAudioEncoder();
	obs_encoder_set_video(videoEncoder, obs_output_video(obsOutput));
	obs_encoder_set_audio(audioEncoder, obs_output_audio(obsOutput));
	obs_output_set_video_encoder(obsOutput, videoEncoder);
	obs_output_set_audio_encoder(obsOutput, audioEncoder, 0);
}

bool RtspOutputHelper::Start()
{
	return obs_output_start(obsOutput);
}

void RtspOutputHelper::Stop()
{
	obs_output_stop(obsOutput);
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

bool RtspOutputHelper::IsActive()
{
	return obs_output_active(obsOutput);
}

void RtspOutputHelper::CreateVideoEncoder()
{
	releaseEncoder(&videoEncoder);
	if (outputSettings.adv_out) {
		videoEncoder = obs_get_encoder_by_name("streaming_h264");
		obs_encoder_set_scaled_size(videoEncoder,
					    outputSettings.rescale_cx,
					    outputSettings.rescale_cy);
		return;
	}
	videoEncoder = obs_get_encoder_by_name("simple_h264_stream");
}

void RtspOutputHelper::CreateAudioEncoder()
{
	releaseEncoder(&audioEncoder);
	if (outputSettings.adv_out) {
		audioEncoder = obs_get_encoder_by_name("avc_aac_stream");
		return;
	}
	audioEncoder = obs_get_encoder_by_name("simple_aac");
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

void RtspOutputHelper::releaseEncoder(obs_encoder_t **encoder)
{
	obs_encoder_release(*encoder);
	*encoder = nullptr;
}

