#ifdef OBS_RTSPSERVER_ENABLE_UI
#ifndef RTSP_PROPERTIES_H
#define RTSP_PROPERTIES_H

#include <QDialog>
#include <obs-module.h>
#include <obs-frontend-api.h>
#include "../rtsp_output_helper.h"

namespace Ui {
class RtspProperties;
}

class RtspProperties : public QDialog {
	Q_OBJECT

public:
	explicit RtspProperties(std::string rtspOutputName, QWidget *parent = 0);
	~RtspProperties();

public Q_SLOTS:
	virtual int exec();

private Q_SLOTS:
	void onPushButtonStartClicked();
	void onPushButtonStopClicked();
	void onPushButtonAddressCopyClicked() const;
	void onCheckBoxEnableMulticastClicked(int checked) const;
	void onSpinBoxPortValueChanged(int value) const;
	void onLineEditUrlSuffixValueChanged(const QString &value) const;
	void onCheckBoxEnableAuthenticationClicked(bool checked) const;
	void onLineEditRealmTextChanged(const QString &value) const;
	void onLineEditUsernameTextChanged(const QString &value) const;
	void onLineEditPasswordTextChanged(const QString &value) const;

	void onStatusTimerTimeout();

	void onButtonStatusChanging(bool outputStarted, bool outputStopped) const;
	void onStatusTimerStatusChanging(bool start);
	void onLabelMessageStatusChanging(bool showError) const;

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

	void LoadConfig(config_t *config) const;
	void SaveConfig(config_t *config) const;
};

#endif // RTSP_PROPERTIES_H
#endif // OBS_RTSPSERVER_ENABLE_UI
