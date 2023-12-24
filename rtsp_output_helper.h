#ifndef RTSP_OUTPUT_HELPER_H
#define RTSP_OUTPUT_HELPER_H

#include <mutex>
#include <vector>
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
	static RtspOutputHelper *CreateRtspOutput(obs_data_t *settings, obs_data_t *hotkey);
	void UpdateSettings(obs_data_t *settings) const;
	obs_data_t *GetSettings() const;
	void UpdateEncoder();
	bool Start() const;
	void Stop() const;
	std::string GetLastError() const;
	obs_data_t *HotkeysSave() const;
	void SignalConnect(const char *signal, signal_callback_t callback,
			   void *data) const;
	void SignalDisconnect(const char *signal, signal_callback_t callback,
			      void *data) const;
	std::string GetOutputName() const;
	uint64_t GetTotalBytes() const;
	int GetTotalFrames() const;
	int GetFramesDropped() const;
	bool IsActive() const;

private:
	RtspOutputHelper(obs_output_t *obsOutput);
	void CreateVideoEncoder();
	void CreateAudioEncoder();
	void GetBaseConfig();
	static void OnPreStartSignal(void *data, calldata_t *cd);
	obs_output_t *obsOutput;
	struct rtsp_output_settings outputSettings;
	obs_encoder_t *videoEncoder = nullptr;
	std::vector<obs_encoder_t *> audioEncoders;
};

#endif // RTSP_OUTPUT_HELPER_H
