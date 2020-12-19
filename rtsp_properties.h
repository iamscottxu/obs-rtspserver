#ifndef RTSP_PROPERTIES_H
#define RTSP_PROPERTIES_H

#include <memory>
#include <QDialog>
#include <obs-module.h>
#include "my_rtsp_output.h"

namespace Ui {
class RtspProperties;
}

class RtspProperties : public QDialog {
	Q_OBJECT

public:
	explicit RtspProperties(std::string rtspOutputName, QWidget *parent = 0);
	~RtspProperties();

private Q_SLOTS:
	void onButtonAddressCopy();
	void onStart();
	void onStop();
	void onEnableOptions(bool startEnable, bool stopRnable);
	void onShowWarning(bool show);

Q_SIGNALS:
	void enableOptions(bool startEnable, bool stopRnable);
	void showWarning(bool show);

private:
	Ui::RtspProperties *ui;
	signal_handler_t *signalHandler;
	RtspOutputHelper *rtspOutputHelper;
	void UpdateParameter();
	void SaveSetting();
	static void OnStartSignal(void *data, calldata_t *cd);
	static void OnStopSignal(void *data, calldata_t *cd);
	void showEvent(QShowEvent *event);
	void closeEvent(QCloseEvent *event);
};

#endif // RTSP_PROPERTIES_H
