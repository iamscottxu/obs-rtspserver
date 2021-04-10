#include <obs-module.h>
#include <obs-frontend-api.h>
#include <util/config-file.h>
#include <QMainWindow>
#include <QAction>
#include <mutex>
#include <net/Logger.h>
#include "helper.h"
#include "rtsp_output_helper.h"
#include "rtsp_output.h"
#include "rtsp_properties.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-rtspserver", "en-US")

void obs_frontend_event(enum obs_frontend_event event, void *ptr);
void rtsp_output_auto_start(RtspOutputHelper *rtspOutputHelper);
void rtsp_output_stop(RtspOutputHelper *rtspOutputHelper);
void rtsp_output_save_hotkey_settings(RtspOutputHelper *rtspOutputHelper);
void server_log_write_callback(xop::Priority priority, std::string info);

const char *obs_module_name(void)
{
	return obs_module_text("RtspServer");
}

const char *obs_module_description(void)
{
	return obs_module_text("RstpServer.Description");
}

bool obs_module_load(void)
{
	xop::Logger::instance().setWriteCallback(server_log_write_callback);
	rtsp_output_register();

	RtspOutputHelper *rtspOutputHelper;
	{
		auto *data = rtsp_output_read_data();
		auto *config = rtsp_properties_open_config();
		const char *str = nullptr;
		str = config_get_string(config, HOTKEY_CONFIG_SECTIION,
					"RtspOutput");
		obs_data_t *hotkey = obs_data_create_from_json(str);
		rtspOutputHelper = RtspOutputHelper::CreateRtspOutput(data, hotkey);
		obs_data_release(hotkey);
		config_close(config);
		obs_data_release(data);
	}

	auto mainWindow = (QMainWindow *)obs_frontend_get_main_window();
	auto action = (QAction *)obs_frontend_add_tools_menu_qaction(
		obs_module_text("RtspServer"));

	obs_frontend_push_ui_translation(obs_module_get_string);
	auto rtspProperties = new RtspProperties(rtspOutputHelper->GetOutputName(), mainWindow);
	obs_frontend_pop_ui_translation();

	action->connect(action, &QAction::triggered, rtspProperties, &QDialog::exec);

	obs_frontend_add_event_callback(obs_frontend_event, rtspOutputHelper);

	return true;
}

void obs_module_unload(void)
{
	obs_frontend_remove_event_callback(obs_frontend_event, nullptr);
}

void obs_frontend_event(enum obs_frontend_event event, void *ptr)
{
	auto rtspOutputHelper = (RtspOutputHelper *)ptr;
	switch (event) {
	case OBS_FRONTEND_EVENT_FINISHED_LOADING:
		rtsp_output_auto_start(rtspOutputHelper);
		break;
	case OBS_FRONTEND_EVENT_EXIT:
		rtsp_output_stop(rtspOutputHelper);
		rtsp_output_save_hotkey_settings(rtspOutputHelper);
		delete rtspOutputHelper;
		break;
	}
}

void rtsp_output_auto_start(RtspOutputHelper *rtspOutputHelper)
{
	auto *config = rtsp_properties_open_config();
	auto autoStart = false;
	if (config) {
		autoStart =
			config_get_bool(config, CONFIG_SECTIION, "AutoStart");
		config_close(config);
	}
	if (autoStart)
		rtspOutputHelper->Start();
}

void rtsp_output_stop(RtspOutputHelper *rtspOutputHelper)
{
	rtspOutputHelper->Stop();
}

void rtsp_output_save_hotkey_settings(RtspOutputHelper *rtspOutputHelper)
{
	auto *data = rtspOutputHelper->HotkeysSave();
	auto *str = obs_data_get_json(data);
	auto *config = rtsp_properties_open_config();
	config_set_string(config, HOTKEY_CONFIG_SECTIION, "RtspOutput", str);
	config_save(config);
	config_close(config);
}

void server_log_write_callback(xop::Priority priority, std::string info)
{
	switch (priority) {
	case xop::LOG_DEBUG:
		blog(LOG_DEBUG, "[rtsp-server] %s", info.c_str());
		break;
	case xop::LOG_STATE:
		blog(LOG_INFO, "[rtsp-server] %s", info.c_str());
		break;
	case xop::LOG_INFO:
		blog(LOG_INFO, "[rtsp-server] %s", info.c_str());
		break;
	case xop::LOG_WARNING:
		blog(LOG_WARNING, "[rtsp-server] %s", info.c_str());
		break;
	case xop::LOG_ERROR:
		blog(LOG_ERROR, "[rtsp-server] %s", info.c_str());
		break;
	default:
		break;
	}
}
