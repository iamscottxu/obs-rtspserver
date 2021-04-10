#include <obs-frontend-api.h>
#include <util/config-file.h>
#include <QMainWindow>
#include <QClipboard>
#include <QString>
#include <QTimer>
#include <QCloseEvent>
#include "rtsp_output.h"
#include "rtsp_properties.h"
#include "ui_rtsp_properties.h"
#include "helper.h"

RtspProperties::RtspProperties(std::string rtspOutputName, QWidget *parent)
	: QDialog(parent),
	  ui(new Ui::RtspProperties),
	  statusTimer(new QTimer(this))
{
	ui->setupUi(this);
	connect(ui->pushButtonStart, &QPushButton::clicked, this,
		&RtspProperties::onPushButtonStartClicked);
	connect(ui->pushButtonStop, &QPushButton::clicked, this,
		&RtspProperties::onPushButtonStopClicked);
	connect(ui->pushButtonAddressCopy, &QPushButton::clicked, this,
		&RtspProperties::onPushButtonAddressCopyClicked);
	connect(statusTimer, &QTimer::timeout, this,
		&RtspProperties::onStatusTimerTimeout);
	connect(this, &RtspProperties::enableOptions, this,
		&RtspProperties::onEnableOptions);
	connect(this, &RtspProperties::showWarning, this,
		&RtspProperties::onShowWarning);

	rtspOutputHelper = new RtspOutputHelper(rtspOutputName);
	onEnableOptions(!rtspOutputHelper->IsActive(),
			rtspOutputHelper->IsActive());
	rtspOutputHelper->SignalConnect("start", OnOutputStart, this);
	rtspOutputHelper->SignalConnect("stop", OnOutputStop, this);

	{
		auto config = rtsp_properties_open_config();
		LoadConfig(config);
		config_close(config);
	}
}

RtspProperties::~RtspProperties()
{
	rtspOutputHelper->SignalDisconnect("start", OnOutputStart, this);
	rtspOutputHelper->SignalDisconnect("stop", OnOutputStop, this);
	delete ui;
	delete rtspOutputHelper;
}

void RtspProperties::showEvent(QShowEvent *event)
{
	auto setting = rtspOutputHelper->GetSettings();
	LoadSetting(setting);
	obs_data_release(setting);
}

void RtspProperties::closeEvent(QCloseEvent *event)
{
	if (this->isHidden())
		return;
	{
		auto config = rtsp_properties_open_config();
		SaveConfig(config);
		config_close(config);
	}
	{
		auto setting = rtspOutputHelper->GetSettings();
		UpdateParameter(setting);
		rtsp_output_save_data(setting);
		obs_data_release(setting);
	}
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
	else
		ui->labelMessage->setText("");
}

void RtspProperties::onStatusTimerTimeout()
{
	auto totalBytes = rtspOutputHelper->GetTotalBytes();
	auto bitps = (totalBytes - lastTotalBytes) * 8;
	lastTotalBytes = totalBytes;
	if (bitps < 0)
		return;
	ui->labelTotalData->setText(
		rtsp_properties_get_data_volume_display(totalBytes).c_str());
	ui->labelBitrate->setText(QString("%1 kb/s").arg(
		bitps / 1000 + (bitps % 1000 >= 500 ? 1 : 0)));
}

void RtspProperties::onPushButtonAddressCopyClicked()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString url = "rtsp://localhost:";
	url.append(ui->spinBoxPort->text());
	url.append("/live");
	clipboard->setText(url);
}

void RtspProperties::onPushButtonStartClicked()
{
	auto data = rtspOutputHelper->GetSettings();
	UpdateParameter(data);
	obs_data_release(data);
	showWarning(!rtspOutputHelper->Start());
}

void RtspProperties::onPushButtonStopClicked()
{
	rtspOutputHelper->Stop();
	enableOptions(false, false);
}

void RtspProperties::OnOutputStart(void *data, calldata_t *cd)
{
	auto page = (RtspProperties *)data;
	page->enableOptions(false, true);
	page->lastTotalBytes = 0;
	page->statusTimer->start(1000);
}

void RtspProperties::OnOutputStop(void *data, calldata_t *cd)
{
	auto page = (RtspProperties *)data;
	auto code = calldata_int(cd, "code");
	if (code != OBS_OUTPUT_SUCCESS)
		page->showWarning(true);
	page->enableOptions(true, false);
	page->statusTimer->stop();
	page->ui->labelTotalData->setText("0.0 MB");
	page->ui->labelBitrate->setText("0 kb/s");
}

void RtspProperties::LoadSetting(obs_data_t *setting)
{
	ui->spinBoxPort->setValue(obs_data_get_int(setting, "port"));
	auto username = std::string(
		obs_data_get_string(setting, "authentication_username"));
	auto realm = std::string(
		obs_data_get_string(setting, "authentication_realm"));
	auto password = std::string(
		obs_data_get_string(setting, "authentication_password"));
	auto enbledAuth = false;
	if (!username.empty() && !realm.empty())
		enbledAuth = obs_data_get_bool(setting, "authentication");

	ui->lineEditUsername->setText(username.c_str());
	ui->lineEditRealm->setText(realm.c_str());
	ui->lineEditPassword->setText(password.c_str());
	ui->checkBoxEnableAuthentication->setChecked(enbledAuth);

	ui->labelUsername->setEnabled(enbledAuth);
	ui->labelRealm->setEnabled(enbledAuth);
	ui->labelPassword->setEnabled(enbledAuth);
	ui->lineEditUsername->setEnabled(enbledAuth);
	ui->lineEditRealm->setEnabled(enbledAuth);
	ui->lineEditPassword->setEnabled(enbledAuth);
}

void RtspProperties::UpdateParameter(obs_data_t *setting)
{
	obs_data_set_int(setting, "port", ui->spinBoxPort->value());
	auto username = ui->lineEditUsername->text().toStdString();
	auto realm = ui->lineEditRealm->text().toStdString();
	auto password = ui->lineEditPassword->text().toStdString();
	auto enbledAuth = false;
	if (!username.empty() && !realm.empty())
		enbledAuth = ui->checkBoxEnableAuthentication->isChecked();

	obs_data_set_bool(setting, "authentication", enbledAuth);
	obs_data_set_string(setting, "authentication_username",
			    username.c_str());
	obs_data_set_string(setting, "authentication_realm", realm.c_str());
	obs_data_set_string(setting, "authentication_password",
			    password.c_str());

	rtspOutputHelper->UpdateSettings(setting);
}

void RtspProperties::LoadConfig(config_t *config)
{
	auto autoStart = config_get_bool(config, CONFIG_SECTIION, "AutoStart");
	ui->checkBoxAuto->setChecked(autoStart);
}

void RtspProperties::SaveConfig(config_t *config)
{
	if (!config)
		return;

	auto autoStart = ui->checkBoxAuto->isChecked();
	config_set_bool(config, CONFIG_SECTIION, "AutoStart", autoStart);
	config_save(config);
}
