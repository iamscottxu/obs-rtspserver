#include <queue>
#include <obs-module.h>
#include <util/config-file.h>
#include <util/platform.h>

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

#endif // RTSP_HELPER_H
