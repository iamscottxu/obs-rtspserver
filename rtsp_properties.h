#ifndef RTSP_PROPERTIES_H
#define RTSP_PROPERTIES_H

#include <QDialog>
#include <obs-module.h>
#include <obs-frontend-api.h>
#include "rtsp_output_helper.h"

namespace Ui {
class RtspProperties;
}

class RtspProperties : public QDialog {
	Q_OBJECT

public:
	explicit RtspProperties(std::string rtspOutputName, QWidget *parent = 0);
	~RtspProperties();

private Q_SLOTS:
	void onPushButtonAddressCopyClicked();
	void onPushButtonStartClicked();
	void onPushButtonStopClicked();
	void onStatusTimerTimeout();
	void onEnableOptions(bool startEnable, bool stopRnable);
	void onShowWarning(bool show);
	void onChangeStatusTimerStatus(bool start);

Q_SIGNALS:
	void enableOptions(bool startEnable, bool stopRnable);
	void showWarning(bool show);
	void changeStatusTimerStatus(bool start);

private:
	Ui::RtspProperties *ui;
	QTimer *statusTimer;

	signal_handler_t *signalHandler;
	RtspOutputHelper *rtspOutputHelper;

	uint64_t lastTotalBytes;

	void showEvent(QShowEvent *event);
	void closeEvent(QCloseEvent *event);

	static void OnOutputStart(void *data, calldata_t *cd);
	static void OnOutputStop(void *data, calldata_t *cd);

	void LoadSetting(obs_data_t *setting);
	void UpdateParameter(obs_data_t *setting);

	void LoadConfig(config_t *config);
	void SaveConfig(config_t *config);
};

#endif // RTSP_PROPERTIES_H
