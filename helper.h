#include <vector>
#include <string>
#include <obs-module.h>
#include <util/config-file.h>
#include <util/platform.h>
#include <obs-avc.h>

#pragma once

#define CONFIG_SECTIION "RstpOutput"
#define HOTKEY_CONFIG_SECTIION "Hotkeys"

enum encoder_codec { UNKNOW = 0, H264 = 1, HEVC = 2, AV1 = 3, AAC = 4 };

[[maybe_unused]] static bool make_config_dir()
{
	auto path = obs_module_config_path("");
	auto ret = os_mkdirs(path);
	bfree(path);
	return ret == MKDIR_SUCCESS || ret == MKDIR_EXISTS;
}

[[maybe_unused]] static obs_data_t *rtsp_output_read_data()
{
	obs_data_t *data;
	auto path = obs_module_config_path("rtsp_output.json");
	data = obs_data_create_from_json_file_safe(path, "bak");
	bfree(path);
	return data;
}

[[maybe_unused]] static bool rtsp_output_save_data(obs_data_t *data)
{
	if (!make_config_dir())
		return false;
	auto path = obs_module_config_path("rtsp_output.json");
	auto ret = obs_data_save_json_safe(data, path, "tmp", "bak");
	bfree(path);
	return ret;
}

[[maybe_unused]] static config_t *rtsp_properties_open_config()
{
	if (!make_config_dir())
		return nullptr;
	auto path = obs_module_config_path("config.ini");
	config_t *config;
	auto ret = config_open(&config, path, CONFIG_OPEN_ALWAYS);
	bfree(path);
	if (ret)
		return nullptr;
	config_set_default_bool(config, CONFIG_SECTIION, "AutoStart", false);
	config_set_default_bool(config, CONFIG_SECTIION, "AudioTrack1", true);
	config_set_default_bool(config, CONFIG_SECTIION, "AudioTrack2", false);
	config_set_default_bool(config, CONFIG_SECTIION, "AudioTrack3", false);
	config_set_default_bool(config, CONFIG_SECTIION, "AudioTrack4", false);
	config_set_default_bool(config, CONFIG_SECTIION, "AudioTrack5", false);
	config_set_default_bool(config, CONFIG_SECTIION, "AudioTrack6", false);
	return config;
}

[[maybe_unused]] static std::string string_format(char const *format, ...)
{
	va_list argp;
	va_start(argp, format);
	auto size = (size_t)vsnprintf(nullptr, 0, format, argp) + 1;
	va_end(argp);
	auto buf = std::vector<char>(size);
	va_start(argp, format);
	vsnprintf(buf.data(), size, format, argp);
	va_end(argp);
	return std::string(buf.data(), buf.data() + size - 1);
}

[[maybe_unused]] static std::string rtsp_properties_get_data_volume_display(uint64_t total_bytes)
{
	const uint64_t kb = 1024;
	const uint64_t mb = kb * 1024;
	const uint64_t gb = mb * 1024;
	const uint64_t tb = gb * 1024;
	if (total_bytes == 0)
		return "0.0 MB";
	if (total_bytes < kb) {
		return string_format("%lu bytes", total_bytes);
	}
	if (total_bytes < mb) {
		return string_format("%.1f KB", double(total_bytes) / kb);
	}
	if (total_bytes < gb) {
		return string_format("%.1f MB", double(total_bytes) / mb);
	}
	if (total_bytes < tb) {
		return string_format("%.1f GB", double(total_bytes) / gb);
	}
	return string_format("%.1f TB", double(total_bytes) / tb);
}

[[maybe_unused]] static encoder_codec get_encoder_codec(const obs_encoder_t *encoder)
{
	const char *const codec = obs_encoder_get_codec(encoder);
	if (strcmp(codec, "h264") == 0) {
		return encoder_codec::H264;
	}
	if (strcmp(codec, "hevc") == 0) {
		return encoder_codec::HEVC;
	}
	if (strcmp(codec, "av1") == 0) {
		return encoder_codec::AV1;
	}
	if (strcmp(codec, "aac") == 0) {
		return encoder_codec::AAC;
	}
	return UNKNOW;
}
