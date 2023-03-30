#ifdef OBS_RTSPSERVER_ENABLE_UI
#include "rtsp_frontend.h"

#include <util/config-file.h>
#include <mutex>
#include "helper.h"
#include "rtsp_output_helper.h"
#include "ui/rtsp_properties.hpp"
#include <obs-frontend-api.h>
#include <QMainWindow>
#include <QAction>

void obs_frontend_event(enum obs_frontend_event event, void *ptr);
void rtsp_output_auto_start(RtspOutputHelper *rtspOutputHelper);
void rtsp_output_stop(RtspOutputHelper *rtspOutputHelper);
void rtsp_output_save_settings(RtspOutputHelper *rtspOutputHelper);
void rtsp_output_save_hotkey_settings(RtspOutputHelper *rtspOutputHelper);

void rtsp_frontend_load()
{
	RtspOutputHelper *rtspOutputHelper;
	{
		auto *settings = rtsp_output_read_data();
		auto *config = rtsp_properties_open_config();
		const char *str = nullptr;
		str = config_get_string(config, HOTKEY_CONFIG_SECTIION,
					"RtspOutput");
		obs_data_t *hotkey = nullptr;
		if (str)
			hotkey = obs_data_create_from_json(str);
		rtspOutputHelper =
			RtspOutputHelper::CreateRtspOutput(settings, hotkey);
		obs_data_release(hotkey);
		config_close(config);
		obs_data_release(settings);
	}

	const auto mainWindow =
		static_cast<QMainWindow *>(obs_frontend_get_main_window());
	const auto action =
		static_cast<QAction *>(obs_frontend_add_tools_menu_qaction(
			obs_module_text("RtspServer")));

	obs_frontend_push_ui_translation(obs_module_get_string);
	const auto rtspProperties = new RtspProperties(
		rtspOutputHelper->GetOutputName(), mainWindow);
	obs_frontend_pop_ui_translation();

	QAction::connect(action, &QAction::triggered, rtspProperties,
			 &QDialog::exec);

	obs_frontend_add_event_callback(obs_frontend_event, rtspOutputHelper);
}

void rtsp_frontend_unload()
{
	obs_frontend_remove_event_callback(obs_frontend_event, nullptr);
}

void obs_frontend_event(enum obs_frontend_event event, void* ptr)
{
	const auto rtspOutputHelper = static_cast<RtspOutputHelper *>(ptr);
	switch (event) {
	case OBS_FRONTEND_EVENT_FINISHED_LOADING:
		rtsp_output_auto_start(rtspOutputHelper);
		break;
	case OBS_FRONTEND_EVENT_EXIT:
		rtsp_output_stop(rtspOutputHelper);
		rtsp_output_save_settings(rtspOutputHelper);
		rtsp_output_save_hotkey_settings(rtspOutputHelper);
		delete rtspOutputHelper;
		break;
	default:;
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

void rtsp_output_save_settings(RtspOutputHelper *rtspOutputHelper)
{
	auto *data = rtspOutputHelper->GetSettings();
	rtsp_output_save_data(data);
}

#endif // OBS_RTSPSERVER_ENABLE_UI
