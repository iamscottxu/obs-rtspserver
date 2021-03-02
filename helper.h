#include <queue>
#include <obs-module.h>
#include <util/config-file.h>
#include <util/platform.h>
#include <obs-avc.h>

#ifndef RTSP_HELPER_H
#define RTSP_HELPER_H

#define CONFIG_SECTIION "RstpOutput"

static bool make_config_dir()
{
	auto path = obs_module_config_path("");
	auto ret = os_mkdirs(path);
	bfree(path);
	return ret == MKDIR_SUCCESS || ret == MKDIR_EXISTS;
}

static obs_data_t *rtsp_output_read_data(bool create = false)
{
	obs_data_t *data;
	if (create) {
		data = obs_output_defaults("rtsp_output");
	}  else {
		auto path = obs_module_config_path("rtsp_output.json");
		data = obs_data_create_from_json_file_safe(path, "bak");
		bfree(path);
		if (data == nullptr)
			data = rtsp_output_read_data(true);
	}
	return data;
}

static bool rtsp_output_save_data(obs_data_t *data)
{
	if (!make_config_dir())
		return false;
	auto path = obs_module_config_path("rtsp_output.json");
	auto ret = obs_data_save_json_safe(data, path, "tmp", "bak");
	bfree(path);
	return ret;
}

static config_t *rtsp_properties_open_config()
{
	if (!make_config_dir())
		return nullptr;
	auto path = obs_module_config_path("config.ini");
	config_t *config;
	auto ret = config_open(&config, path, CONFIG_OPEN_ALWAYS);
	bfree(path);
	config_set_default_bool(config, CONFIG_SECTIION, "AutoStart", false);
	return config;
}

static void rtsp_output_avc_get_sps_pps(const uint8_t *data, size_t size,
					const uint8_t **sps, size_t *sps_size,
					const uint8_t **pps, size_t *pps_size)
{
	const uint8_t *nal_start, *nal_end;
	const uint8_t *end = data + size;
	int type;

	nal_start = obs_avc_find_startcode(data, end);
	while (true) {
		while (nal_start < end && !*(nal_start++))
			;

		if (nal_start == end)
			break;

		nal_end = obs_avc_find_startcode(nal_start, end);

		type = nal_start[0] & 0x1F;
		if (type == OBS_NAL_SPS) {
			*sps = nal_start;
			*sps_size = nal_end - nal_start;
		} else if (type == OBS_NAL_PPS) {
			*pps = nal_start;
			*pps_size = nal_end - nal_start;
		}

		nal_start = nal_end;
	}
}
#endif // RTSP_HELPER_H
