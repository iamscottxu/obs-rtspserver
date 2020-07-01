#ifndef RTSP_PROPERTIES_H
#define RTSP_PROPERTIES_H

#include <QDialog>
#include <obs-module.h>
#include "my_rtsp_output.h"

namespace Ui {
class RtspProperties;
}

class RtspProperties : public QDialog {
	Q_OBJECT

public:
	explicit RtspProperties(QWidget *parent = 0);
	~RtspProperties();
	void SetVisable();
	void showEvent(QShowEvent *event);
	void closeEvent(QCloseEvent *event);
	void EnableOptions(bool enable);
	void ShowWarning(bool show);
	MyRtspOutput *GetMyRtspOutput();

private Q_SLOTS:
	void onButtonAddressCopy();
	void onStart();
	void onStop();

private:
	Ui::RtspProperties *ui;
	signal_handler_t *signalHandler;
	std::unique_ptr<MyRtspOutput> myRtspOutput;
	void UpdateParameter();
	void SaveSetting();
	static void OnStartSignal(void *data, calldata_t *cd);
	static void OnStopSignal(void *data, calldata_t *cd);
};

#endif // RTSP_PROPERTIES_H
