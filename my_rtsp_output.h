#ifndef MY_RTSP_OUTPUT_H
#define MY_RTSP_OUTPUT_H

#include <mutex>
#include <obs-module.h>

struct rtsp_output_settings {
	bool adv_out = false;
	uint32_t rescale_cx = 0;
	uint32_t rescale_cy = 0;
};

class RtspOutputHelper {
public:
	RtspOutputHelper(std::string outputName);
	~RtspOutputHelper();
	static RtspOutputHelper *CreateRtspOutput(obs_data_t *settings);
	void UpdateSettings(obs_data_t *settings);
	void UpdateEncoder();
	bool Start();
	void Stop();
	void SignalConnect(const char *signal, signal_callback_t callback,
			   void *data);
	void SignalDisconnect(const char *signal, signal_callback_t callback,
			      void *data);
	std::string GetOutputName();
	bool IsActive();

private:
	RtspOutputHelper(obs_output_t *obsOutput);
	void CreateVideoEncoder();
	void CreateAudioEncoder();
	void GetBaseConfig();
	static void releaseEncoder(obs_encoder_t **encoder);
	obs_output_t *obsOutput;
	struct rtsp_output_settings outputSettings;
	obs_encoder_t *videoEncoder = nullptr;
	obs_encoder_t *audioEncoder = nullptr;
};

#endif // MY_RTSP_OUTPUT_H
