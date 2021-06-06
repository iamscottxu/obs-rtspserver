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
	void onPushButtonStartClicked();
	void onPushButtonStopClicked();
	void onPushButtonAddressCopyClicked();
	void onSpinBoxPortValueChanged(int value);
	void onCheckBoxEnableAuthenticationClicked(bool checked);
	void onLineEditRealmTextChanged(const QString value);
	void onLineEditUsernameTextChanged(const QString value);
	void onLineEditPasswordTextChanged(const QString value);

	void onStatusTimerTimeout();

	void onButtonStatusChanging(bool outputStarted, bool outputStopped);
	void onStatusTimerStatusChanging(bool start);
	void onLabelMessageStatusChanging(bool showError);

Q_SIGNALS:
	void setButtonStatus(bool outputStarted, bool outputStopped);
	void setStatusTimerStatus(bool start);
	void setLabelMessageStatus(bool showError);

private:
	Ui::RtspProperties *ui;
	QTimer *statusTimer;

	signal_handler_t *signalHandler;
	RtspOutputHelper *rtspOutputHelper;

	uint64_t lastTotalBytes;

	obs_data_t *settings;

	void showEvent(QShowEvent *event);
	void closeEvent(QCloseEvent *event);

	static void OnOutputStart(void *data, calldata_t *cd);
	static void OnOutputStop(void *data, calldata_t *cd);

	void LoadConfig(config_t *config);
	void SaveConfig(config_t *config);
};

#endif // RTSP_PROPERTIES_H
