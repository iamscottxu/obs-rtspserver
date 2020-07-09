#include <obs-module.h>
#include <obs-frontend-api.h>
#include <util/config-file.h>
#include <QMainWindow>
#include <QAction>
#include "helper.h"
#include "my_rtsp_output.h"
#include "rtsp_properties.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-rtspserver", "en-US")

RtspProperties *rtspProperties;

void obs_frontend_event(enum obs_frontend_event event, void *ptr);
void rtsp_output_auto_start();
void rtsp_output_stop();

const char *obs_module_name(void)
{
	return obs_module_text("RtspServer");
}

const char *obs_module_description(void)
{
	return obs_module_text("RstpServerDescription");
}

bool obs_module_load(void)
{
	QMainWindow *mainWindow = (QMainWindow *)obs_frontend_get_main_window();
	QAction *action = (QAction *)obs_frontend_add_tools_menu_qaction(
		obs_module_text("RtspServer"));

	obs_frontend_push_ui_translation(obs_module_get_string);
	rtspProperties = new RtspProperties(mainWindow);
	rtspProperties->setModal(true);
	obs_frontend_pop_ui_translation();

	auto menu_cb = [] {
		rtspProperties->setVisible(!rtspProperties->isVisible());
	};

	action->connect(action, &QAction::triggered, menu_cb);

	obs_frontend_add_event_callback(obs_frontend_event, nullptr);

	return true;
}

void obs_module_unload(void)
{
	obs_frontend_remove_event_callback(obs_frontend_event, nullptr);
}

void obs_frontend_event(enum obs_frontend_event event, void *ptr)
{
	switch ((int)event) {
	case OBS_FRONTEND_EVENT_FINISHED_LOADING:
		rtsp_output_auto_start();
		break;
	case OBS_FRONTEND_EVENT_EXIT:
		rtsp_output_stop();
		break;
	}
}

void rtsp_output_auto_start()
{
	config_t *config = rtsp_properties_open_config();
	auto autoStart = false;
	if (config) {
		autoStart =
			config_get_bool(config, CONFIG_SECTIION, "AutoStart");
		config_close(config);
	}
	if (!autoStart)
		return;
	rtspProperties->GetMyRtspOutput()->UpdateEncoder();
	rtspProperties->GetMyRtspOutput()->Start();
}

void rtsp_output_stop()
{
	rtspProperties->GetMyRtspOutput()->Stop();
}
