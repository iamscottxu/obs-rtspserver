#include <obs-module.h>
#include <net/Logger.h>
#include "rtsp_output.h"
#include "rtsp_frontend.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-rtspserver", "en-US")

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
	xop::Logger::Instance().SetWriteCallback(server_log_write_callback);
	rtsp_output_register();

#ifdef OBS_RTSPSERVER_ENABLE_UI
	rtsp_frontend_load();
#endif

	return true;
}

void obs_module_unload(void)
{
#ifdef OBS_RTSPSERVER_ENABLE_UI
	rtsp_frontend_unload();
#endif
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
	default: ;
	}
}
