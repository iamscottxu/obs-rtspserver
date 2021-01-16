#include <obs-frontend-api.h>
#include <util/config-file.h>
#include <QMainWindow>
#include <QClipboard>
#include "rtsp_output.h"
#include "rtsp_properties.h"
#include "ui_rtsp_properties.h"
#include "helper.h"

RtspProperties::RtspProperties(std::string rtspOutputName, QWidget *parent)
	: QDialog(parent), ui(new Ui::RtspProperties)
{
	ui->setupUi(this);
	connect(ui->pushButtonStart,&QPushButton::clicked, this, &RtspProperties::onStart);
	connect(ui->pushButtonStop, &QPushButton::clicked, this, &RtspProperties::onStop);
	connect(ui->pushButtonAddressCopy, &QPushButton::clicked, this, &RtspProperties::onButtonAddressCopy);
	connect(this, &RtspProperties::enableOptions, this, &RtspProperties::onEnableOptions);
	connect(this, &RtspProperties::showWarning, this, &RtspProperties::onShowWarning);

	{
		auto config = rtsp_properties_open_config();
		ui->checkBoxAuto->setChecked(
			config_get_bool(config, CONFIG_SECTIION, "AutoStart"));
		config_close(config);
	}

	{
		auto data = rtsp_output_read_data();
		ui->spinBoxPort->setValue(obs_data_get_int(data, "port"));
		obs_data_release(data);
	}

	ui->labelMessage->setVisible(false);

	rtspOutputHelper = new RtspOutputHelper(rtspOutputName);
	onEnableOptions(!rtspOutputHelper->IsActive(),
			rtspOutputHelper->IsActive());

	rtspOutputHelper->SignalConnect("stop", OnStopSignal, this);
	rtspOutputHelper->SignalConnect("starting", OnStartSignal, this);
	rtspOutputHelper->SignalConnect("start", OnStartSignal, this);
	rtspOutputHelper->SignalConnect("stoping", OnStartSignal, this);
}

RtspProperties::~RtspProperties()
{
	rtspOutputHelper->SignalDisconnect("start", OnStartSignal, this);
	rtspOutputHelper->SignalDisconnect("stop", OnStopSignal, this);
	delete ui;
	delete rtspOutputHelper;
}

void RtspProperties::onEnableOptions(bool startEnable, bool stopRnable)
{
	ui->spinBoxPort->setEnabled(startEnable);
	ui->pushButtonStart->setEnabled(startEnable);
	ui->pushButtonStop->setEnabled(stopRnable);
}

void RtspProperties::onShowWarning(bool show)
{
	if (show)
		ui->labelMessage->setText(
			QString(rtspOutputHelper->GetLastError().c_str()));
	ui->labelMessage->setVisible(show);
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
	rtspOutputHelper->UpdateEncoder();
	showWarning(!rtspOutputHelper->Start());
}

void RtspProperties::onStop()
{
	enableOptions(false, false);
	rtspOutputHelper->Stop();
}

void RtspProperties::OnStartSignal(void *data, calldata_t *cd)
{
	auto page = (RtspProperties *)data;
	page->enableOptions(false, true);
}

void RtspProperties::OnStopSignal(void *data, calldata_t *cd)
{
	auto page = (RtspProperties *)data;
	auto code = calldata_int(cd, "code");
	if (code != OBS_OUTPUT_SUCCESS)
		page->showWarning(true);
	page->enableOptions(true, false);
}

void RtspProperties::UpdateParameter()
{
	auto data = rtsp_output_read_data(true);
	obs_data_set_int(data, "port", ui->spinBoxPort->value());
	rtspOutputHelper->UpdateSettings(data);
	obs_data_release(data);
}

void RtspProperties::showEvent(QShowEvent *event) {}

void RtspProperties::closeEvent(QCloseEvent *event)
{
	SaveSetting();
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
	obs_data_set_int(data, "port", ui->spinBoxPort->value());
	rtsp_output_save_data(data);
}
