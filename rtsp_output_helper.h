#ifndef RTSP_OUTPUT_HELPER_H
#define RTSP_OUTPUT_HELPER_H

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
	std::string GetLastError();
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
	static void RtspOutputHelper::OnPreStartSignal(void *data, calldata_t *cd);
	obs_output_t *obsOutput;
	struct rtsp_output_settings outputSettings;
	obs_encoder_t *videoEncoder = nullptr;
	obs_encoder_t *audioEncoder = nullptr;
};

#endif // RTSP_OUTPUT_HELPER_H
