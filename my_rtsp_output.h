#ifndef MY_RTSP_OUTPUT_H
#define MY_RTSP_OUTPUT_H

#include <obs-module.h>

struct rtsp_output_settings {
	bool adv_out = false;
	uint32_t rescale_cx = 0;
	uint32_t rescale_cy = 0;
};

class MyRtspOutput {
public:
	MyRtspOutput(obs_data_t *settings);
	~MyRtspOutput();
	void UpdateSettings(obs_data_t *settings);
	void UpdateEncoder();
	bool Start();
	void Stop();
	void SignalConnect(const char *signal, signal_callback_t callback,
			   void *data);
	void SignalDisconnect(const char *signal, signal_callback_t callback,
			      void *data);
	bool IsRunning();

private:
	void CreateVideoEncoder();
	void CreateAudioEncoder();
	void GetBaseConfig();
	static void releaseEncoder(obs_encoder_t **encoder);
	static void onStartSignal(void *data, calldata_t *cd);
	static void onStopSignal(void *data, calldata_t *cd);
	obs_output_t *rtspOut;
	bool isRunning = false;
	struct rtsp_output_settings outputSettings;
	obs_encoder_t *videoEncoder = nullptr;
	obs_encoder_t *audioEncoder = nullptr;
};

#endif // MY_RTSP_OUTPUT_H
