#include <obs-frontend-api.h>
#include <util/config-file.h>
#include <QMainWindow>
#include <QClipboard>
#include "rtsp_output.h"
#include "rtsp_properties.h"
#include "ui_rtsp_properties.h"
#include "helper.h"

RtspProperties::RtspProperties(QWidget *parent)
	: QDialog(parent), ui(new Ui::RtspProperties)
{
	ui->setupUi(this);
	connect(ui->pushButtonStart, SIGNAL(clicked()), this, SLOT(onStart()));
	connect(ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(onStop()));
	connect(ui->pushButtonAddressCopy, SIGNAL(clicked()), this,
		SLOT(onButtonAddressCopy()));

	{
		obs_data_t *data = rtsp_output_read_data();
		myRtspOutput = std::make_unique<MyRtspOutput>(data);
		obs_data_release(data);
	}

	myRtspOutput->SignalConnect("start", OnStartSignal, this);
	myRtspOutput->SignalConnect("stop", OnStopSignal, this);

	config_t *config = rtsp_properties_open_config();
	auto autoStart = config_get_bool(config, CONFIG_SECTIION, "AutoStart");
	config_close(config);

	obs_data_t *data = rtsp_output_read_data();
	int port = obs_data_get_int(data, "port");
	obs_data_release(data);

	ui->checkBoxAuto->setChecked(autoStart);
	ui->spinBoxPort->setValue(port);
	ui->labelMessage->setStyleSheet("QLabel { color : red; }");
	ui->labelMessage->setVisible(false);
	ui->pushButtonStop->setEnabled(false);
}

RtspProperties::~RtspProperties()
{
	myRtspOutput->SignalDisconnect("start", OnStartSignal, this);
	myRtspOutput->SignalDisconnect("stop", OnStopSignal, this);
	delete ui;
}

void RtspProperties::EnableOptions(bool enable)
{
	ui->spinBoxPort->setEnabled(enable);
	ui->pushButtonStart->setEnabled(enable);
	ui->pushButtonStop->setEnabled(!enable);
}

void RtspProperties::ShowWarning(bool show)
{
	ui->labelMessage->setVisible(show);
}

MyRtspOutput *RtspProperties::GetMyRtspOutput()
{
	return myRtspOutput.get();
}

void RtspProperties::onButtonAddressCopy()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString url = "rtsp://localhost:";
	url.append(ui->spinBoxPort->text());
	url.append("/live");
	clipboard->setText(url);
}

void RtspProperties::onStart()
{
	UpdateParameter();
	myRtspOutput->UpdateEncoder();
	ShowWarning(!myRtspOutput->Start());
}

void RtspProperties::onStop()
{
	myRtspOutput->Stop();
}

void RtspProperties::OnStartSignal(void *data, calldata_t *cd)
{
	auto page = (RtspProperties *)data;
	page->EnableOptions(false);
}

void RtspProperties::OnStopSignal(void *data, calldata_t *cd)
{
	auto page = (RtspProperties *)data;
	page->EnableOptions(true);
}

void RtspProperties::UpdateParameter()
{
	SaveSetting();
	auto data = rtsp_output_read_data();
	myRtspOutput->UpdateSettings(data);
	obs_data_release(data);
}

void RtspProperties::showEvent(QShowEvent *event)
{
	setModal(true);
}

void RtspProperties::closeEvent(QCloseEvent *event)
{
	SaveSetting();
	setModal(false);
}

void RtspProperties::SaveSetting()
{
	config_t *config = rtsp_properties_open_config();
	if (config) {
		bool autoStart = ui->checkBoxAuto->isChecked();
		config_set_bool(config, CONFIG_SECTIION, "AutoStart",
				autoStart);
		config_save(config);
		config_close(config);
	}
	auto data = rtsp_output_read_data(true);
	int port = ui->spinBoxPort->value();
	obs_data_set_int(data, "port", port);
	rtsp_output_save_data(data);
}
