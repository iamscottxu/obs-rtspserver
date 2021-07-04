#include <obs-frontend-api.h>
#include <util/config-file.h>
#include <QMainWindow>
#include <QClipboard>
#include <QString>
#include <QTimer>
#include <QCloseEvent>
#include <utility>
#include "rtsp_properties.h"
#include "ui_rtsp_properties.h"
#include "helper.h"

RtspProperties::RtspProperties(std::string rtspOutputName, QWidget *parent)
	: QDialog(parent),
	  ui(new Ui::RtspProperties),
	  statusTimer(new QTimer(this)),
	  rtspOutputHelper(new RtspOutputHelper(std::move(rtspOutputName)))
{
	ui->setupUi(this);

	connect(ui->pushButtonStart, &QPushButton::clicked, this,
		&RtspProperties::onPushButtonStartClicked);
	connect(ui->pushButtonStop, &QPushButton::clicked, this,
		&RtspProperties::onPushButtonStopClicked);
	connect(ui->pushButtonAddressCopy, &QPushButton::clicked, this,
		&RtspProperties::onPushButtonAddressCopyClicked);
	connect(ui->spinBoxPort,
		static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, &RtspProperties::onSpinBoxPortValueChanged);
	connect(ui->lineEditUrlSuffix, &QLineEdit::textChanged, this,
		&RtspProperties::onLineEditUrlSuffixValueChanged);
	connect(ui->checkBoxEnableAuthentication, &QCheckBox::clicked, this,
		&RtspProperties::onCheckBoxEnableAuthenticationClicked);
	connect(ui->lineEditRealm, &QLineEdit::textChanged, this,
		&RtspProperties::onLineEditRealmTextChanged);
	connect(ui->lineEditUsername, &QLineEdit::textChanged, this,
		&RtspProperties::onLineEditUsernameTextChanged);
	connect(ui->lineEditPassword, &QLineEdit::textChanged, this,
		&RtspProperties::onLineEditPasswordTextChanged);

	connect(statusTimer, &QTimer::timeout, this,
		&RtspProperties::onStatusTimerTimeout);

	connect(this, &RtspProperties::setButtonStatus, this,
		&RtspProperties::onButtonStatusChanging);
	connect(this, &RtspProperties::setStatusTimerStatus, this,
		&RtspProperties::onStatusTimerStatusChanging);
	connect(this, &RtspProperties::setLabelMessageStatus, this,
		&RtspProperties::onLabelMessageStatusChanging);

	settings = rtspOutputHelper->GetSettings();
	onButtonStatusChanging(!rtspOutputHelper->IsActive(),
			       rtspOutputHelper->IsActive());
	rtspOutputHelper->SignalConnect("start", OnOutputStart, this);
	rtspOutputHelper->SignalConnect("stop", OnOutputStop, this);

	{
		const auto config = rtsp_properties_open_config();
		LoadConfig(config);
		config_close(config);
	}
}

RtspProperties::~RtspProperties()
{
	rtspOutputHelper->SignalDisconnect("start", OnOutputStart, this);
	rtspOutputHelper->SignalDisconnect("stop", OnOutputStop, this);
	obs_data_release(settings);
	delete ui;
	delete rtspOutputHelper;
}

int RtspProperties::exec()
{
	if (const auto host = this->parentWidget(); host) {
		const auto hostRect = host->geometry();
		this->move(hostRect.center() - this->rect().center());
	}
	return QDialog::exec();
}

void RtspProperties::onPushButtonStartClicked()
{
	{
		const auto config = rtsp_properties_open_config();
		SaveConfig(config);
		config_close(config);
	}
	setLabelMessageStatus(!rtspOutputHelper->Start());
}

void RtspProperties::onPushButtonStopClicked()
{
	rtspOutputHelper->Stop();
	setButtonStatus(false, false);
}

void RtspProperties::onPushButtonAddressCopyClicked()
{
	QString url = "rtsp://localhost";
	if (ui->spinBoxPort->value() != 554) {
		url.append(":");
		url.append(ui->spinBoxPort->text());
	}
	url.append("/");
	url.append(ui->lineEditUrlSuffix->text());
	QApplication::clipboard()->setText(url);
}

void RtspProperties::onSpinBoxPortValueChanged(int value)
{
	obs_data_set_int(settings, "port", value);
}

void RtspProperties::onLineEditUrlSuffixValueChanged(const QString value)
{
	obs_data_set_string(settings, "url_suffix",
			    value.toStdString().c_str());
}

void RtspProperties::onCheckBoxEnableAuthenticationClicked(bool checked)
{
	obs_data_set_bool(settings, "authentication", checked);
}

void RtspProperties::onLineEditRealmTextChanged(const QString value)
{
	obs_data_set_string(settings, "authentication_realm",
			    value.toStdString().c_str());
}

void RtspProperties::onLineEditUsernameTextChanged(const QString value)
{
	obs_data_set_string(settings, "authentication_username",
			    value.toStdString().c_str());
}

void RtspProperties::onLineEditPasswordTextChanged(const QString value)
{
	obs_data_set_string(settings, "authentication_password",
			    value.toStdString().c_str());
}

void RtspProperties::onStatusTimerTimeout()
{
	const auto totalBytes = rtspOutputHelper->GetTotalBytes();
	const auto bitps = (totalBytes - lastTotalBytes) * 8;
	lastTotalBytes = totalBytes;
	ui->labelTotalData->setText(
		rtsp_properties_get_data_volume_display(totalBytes).c_str());
	ui->labelBitrate->setText(QString("%1 kb/s").arg(
		bitps / 1000 + (bitps % 1000 >= 500 ? 1 : 0)));
}

void RtspProperties::onButtonStatusChanging(bool outputStarted,
					    bool outputStopped)
{
	ui->spinBoxPort->setEnabled(outputStarted);
	ui->lineEditUrlSuffix->setEnabled(outputStarted);
	ui->checkBoxAudioTrack1->setEnabled(outputStarted);
	ui->checkBoxAudioTrack2->setEnabled(outputStarted);
	ui->checkBoxAudioTrack3->setEnabled(outputStarted);
	ui->checkBoxAudioTrack4->setEnabled(outputStarted);
	ui->checkBoxAudioTrack5->setEnabled(outputStarted);
	ui->checkBoxAudioTrack6->setEnabled(outputStarted);
	ui->pushButtonStart->setEnabled(outputStarted);
	ui->pushButtonStop->setEnabled(outputStopped);
}

void RtspProperties::onStatusTimerStatusChanging(bool start)
{
	if (start) {
		lastTotalBytes = 0;
		statusTimer->start(1000);
	} else {
		statusTimer->stop();
		ui->labelTotalData->setText("0.0 MB");
		ui->labelBitrate->setText("0 kb/s");
	}
}

void RtspProperties::onLabelMessageStatusChanging(bool showError)
{
	if (showError)
		ui->labelMessage->setText(
			QString(rtspOutputHelper->GetLastError().c_str()));
	else
		ui->labelMessage->setText("");
}

void RtspProperties::showEvent(QShowEvent *event)
{
	ui->spinBoxPort->blockSignals(true);
	ui->spinBoxPort->setValue(obs_data_get_int(settings, "port"));
	ui->spinBoxPort->blockSignals(false);

	ui->lineEditUrlSuffix->blockSignals(true);
	ui->lineEditUrlSuffix->setText(
		std::string(obs_data_get_string(settings, "url_suffix")).c_str());
	ui->lineEditUrlSuffix->blockSignals(false);

	const auto realm = std::string(
		obs_data_get_string(settings, "authentication_realm"));
	const auto username = std::string(
		obs_data_get_string(settings, "authentication_username"));
	const auto password = std::string(
		obs_data_get_string(settings, "authentication_password"));

	auto enbledAuth = false;
	if (!realm.empty() && !username.empty())
		enbledAuth = obs_data_get_bool(settings, "authentication");

	ui->checkBoxEnableAuthentication->setChecked(enbledAuth);

	ui->lineEditRealm->blockSignals(true);
	ui->lineEditRealm->setText(realm.c_str());
	ui->lineEditRealm->blockSignals(false);

	ui->lineEditUsername->blockSignals(true);
	ui->lineEditUsername->setText(username.c_str());
	ui->lineEditUsername->blockSignals(false);

	ui->lineEditPassword->blockSignals(true);
	ui->lineEditPassword->setText(password.c_str());
	ui->lineEditPassword->blockSignals(false);
}

void RtspProperties::closeEvent(QCloseEvent *event)
{
	if (this->isHidden())
		return;
	{
		const auto config = rtsp_properties_open_config();
		SaveConfig(config);
		config_close(config);
	}
	rtspOutputHelper->UpdateSettings(settings);
}

void RtspProperties::OnOutputStart(void *data, calldata_t *cd)
{
	auto page = static_cast<RtspProperties *>(data);
	page->setButtonStatus(false, true);
	page->setStatusTimerStatus(true);
}

void RtspProperties::OnOutputStop(void *data, calldata_t *cd)
{
	auto page = static_cast<RtspProperties *>(data);
	if (const auto code = calldata_int(cd, "code");
	    code != OBS_OUTPUT_SUCCESS)
		page->setLabelMessageStatus(true);
	page->setButtonStatus(true, false);
	page->setStatusTimerStatus(false);
}

void RtspProperties::LoadConfig(config_t *config)
{
	ui->checkBoxAuto->setChecked(
		config_get_bool(config, CONFIG_SECTIION, "AutoStart"));
	ui->checkBoxAudioTrack1->setChecked(
		config_get_bool(config, CONFIG_SECTIION, "AudioTrack1"));
	ui->checkBoxAudioTrack2->setChecked(
		config_get_bool(config, CONFIG_SECTIION, "AudioTrack2"));
	ui->checkBoxAudioTrack3->setChecked(
		config_get_bool(config, CONFIG_SECTIION, "AudioTrack3"));
	ui->checkBoxAudioTrack4->setChecked(
		config_get_bool(config, CONFIG_SECTIION, "AudioTrack4"));
	ui->checkBoxAudioTrack5->setChecked(
		config_get_bool(config, CONFIG_SECTIION, "AudioTrack5"));
	ui->checkBoxAudioTrack6->setChecked(
		config_get_bool(config, CONFIG_SECTIION, "AudioTrack6"));
}

void RtspProperties::SaveConfig(config_t *config)
{
	if (!config)
		return;

	config_set_bool(config, CONFIG_SECTIION, "AutoStart",
			ui->checkBoxAuto->isChecked());
	config_set_bool(config, CONFIG_SECTIION, "AudioTrack1",
			ui->checkBoxAudioTrack1->isChecked());
	config_set_bool(config, CONFIG_SECTIION, "AudioTrack2",
			ui->checkBoxAudioTrack2->isChecked());
	config_set_bool(config, CONFIG_SECTIION, "AudioTrack3",
			ui->checkBoxAudioTrack3->isChecked());
	config_set_bool(config, CONFIG_SECTIION, "AudioTrack4",
			ui->checkBoxAudioTrack4->isChecked());
	config_set_bool(config, CONFIG_SECTIION, "AudioTrack5",
			ui->checkBoxAudioTrack5->isChecked());
	config_set_bool(config, CONFIG_SECTIION, "AudioTrack6",
			ui->checkBoxAudioTrack6->isChecked());
	config_save(config);
}
