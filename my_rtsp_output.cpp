#include <stdio.h>
#include <obs-module.h>
#include <obs-frontend-api.h>
#include <util/config-file.h>
#include <util/dstr.h>
#include "my_rtsp_output.h"
#include "rtsp_output.h"

MyRtspOutput::MyRtspOutput(obs_data_t *settings)
{
	auto rtsp_output_info = create_output_info();
	obs_register_output(&rtsp_output_info);
	rtspOut =
		obs_output_create("rtsp_output", "RtspOutput", settings, NULL);

	static const char *output_signals[] = {
		"void start()",   "void stop()",           "void pause()",
		"void unpause()", "void error(char* msg)", NULL};

	auto handler = obs_output_get_signal_handler(rtspOut);
	signal_handler_add_array(handler, output_signals);

	this->SignalConnect("start", onStartSignal, this);
	this->SignalConnect("stop", onStopSignal, this);
}

MyRtspOutput::~MyRtspOutput()
{
	this->SignalDisconnect("start", onStartSignal, this);
	this->SignalDisconnect("stop", onStopSignal, this);
	obs_output_release(rtspOut);
	releaseEncoder(&videoEncoder);
	releaseEncoder(&audioEncoder);
}

void MyRtspOutput::UpdateSettings(obs_data_t *settings)
{
	obs_output_update(rtspOut, settings);
}

void MyRtspOutput::UpdateEncoder()
{
	GetBaseConfig();
	CreateVideoEncoder();
	CreateAudioEncoder();
	obs_encoder_set_video(videoEncoder, obs_output_video(rtspOut));
	obs_encoder_set_audio(audioEncoder, obs_output_audio(rtspOut));
	obs_output_set_video_encoder(rtspOut, videoEncoder);
	obs_output_set_audio_encoder(rtspOut, audioEncoder, 0);
}

bool MyRtspOutput::Start()
{
	return obs_output_start(rtspOut);
}

void MyRtspOutput::Stop()
{
	obs_output_stop(rtspOut);
	releaseEncoder(&videoEncoder);
	releaseEncoder(&audioEncoder);
}

void MyRtspOutput::SignalConnect(const char *signal, signal_callback_t callback,
				 void *data)
{
	auto handler = obs_output_get_signal_handler(rtspOut);
	signal_handler_connect(handler, signal, callback, data);
}

void MyRtspOutput::SignalDisconnect(const char *signal,
				    signal_callback_t callback, void *data)
{
	auto handler = obs_output_get_signal_handler(rtspOut);
	signal_handler_disconnect(handler, signal, callback, data);
}

bool MyRtspOutput::IsRunning()
{
	return isRunning;
}

void MyRtspOutput::CreateVideoEncoder()
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

void MyRtspOutput::CreateAudioEncoder()
{
	releaseEncoder(&audioEncoder);
	if (outputSettings.adv_out) {
		audioEncoder = obs_get_encoder_by_name("avc_aac_stream");
		return;
	}
	audioEncoder = obs_get_encoder_by_name("simple_aac");
}

void MyRtspOutput::GetBaseConfig()
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

void MyRtspOutput::releaseEncoder(obs_encoder_t **encoder)
{
	obs_encoder_release(*encoder);
	*encoder = nullptr;
}

void MyRtspOutput::onStartSignal(void *data, calldata_t *cd)
{
	auto page = (MyRtspOutput *)data;
	page->isRunning = true;
}

void MyRtspOutput::onStopSignal(void *data, calldata_t *cd)
{
	auto page = (MyRtspOutput *)data;
	page->isRunning = false;
}
