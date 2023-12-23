#ifdef OBS_RTSPSERVER_ENABLE_UI
#include <obs-frontend-api.h>
#include <util/config-file.h>
#include <QMainWindow>
#include <QClipboard>
#include <QString>
#include <QTimer>
#include <QCloseEvent>
#include <utility>
#include "rtsp_properties.hpp"
#include "ui_rtsp_properties.h"
#include "../helper.h"

RtspProperties::RtspProperties(std::string rtspOutputName, QWidget *parent)
	: QDialog(parent),
	  ui(new Ui::RtspProperties),
	  statusTimer(new QTimer(this)),
	  rtspOutputHelper(new RtspOutputHelper(std::move(rtspOutputName))),
	  settings(rtspOutputHelper->GetSettings())
{
	ui->setupUi(this);

	connect(ui->pushButtonStart, &QPushButton::clicked, this,
		&RtspProperties::onPushButtonStartClicked);
	connect(ui->pushButtonStop, &QPushButton::clicked, this,
		&RtspProperties::onPushButtonStopClicked);
	connect(ui->pushButtonAddressCopy, &QPushButton::clicked, this,
		&RtspProperties::onPushButtonAddressCopyClicked);
	connect(ui->checkBoxEnableMulticast, &QCheckBox::clicked, this,
		&RtspProperties::onCheckBoxEnableMulticastClicked);
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

	//ui->lineEditUrlSuffix->setValidator(new QRegExpValidator(QRegExp("^([-A-Za-z0-9+&@#%=~_|]+)(\\/[-A-Za-z0-9+&@#%=~_|]+)*$"), this));

#ifdef VERSION_STRING
	ui->labelVersion->setText(VERSION_STRING);
#endif

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

void RtspProperties::onPushButtonAddressCopyClicked() const
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

void RtspProperties::onCheckBoxEnableMulticastClicked(const int checked) const
{
	obs_data_set_bool(settings, "multicast", checked);
}

void RtspProperties::onSpinBoxPortValueChanged(const int value) const
{
	obs_data_set_int(settings, "port", value);
}

void RtspProperties::onLineEditUrlSuffixValueChanged(const QString &value) const
{
	//auto rx = QRegExp("^[-A-Za-z0-9+&@#%=~_|]+(/[-A-Za-z0-9+&@#%=~_|]+)*$");
	//if (!rx.exactMatch(value))
		//return;
	obs_data_set_string(settings, "url_suffix",
			    value.toStdString().c_str());
}

void RtspProperties::onCheckBoxEnableAuthenticationClicked(const bool checked) const
{
	obs_data_set_bool(settings, "authentication", checked);
}

void RtspProperties::onLineEditRealmTextChanged(const QString &value) const
{
	obs_data_set_string(settings, "authentication_realm",
			    value.toStdString().c_str());
}

void RtspProperties::onLineEditUsernameTextChanged(const QString &value) const
{
	obs_data_set_string(settings, "authentication_username",
			    value.toStdString().c_str());
}

void RtspProperties::onLineEditPasswordTextChanged(const QString &value) const
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

void RtspProperties::onButtonStatusChanging(const bool outputStarted,
                                            const bool outputStopped) const
{
	ui->checkBoxEnableMulticast->setEnabled(outputStarted);
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

void RtspProperties::onStatusTimerStatusChanging(const bool start)
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

void RtspProperties::onLabelMessageStatusChanging(const bool showError) const
{
	if (showError)
		ui->labelMessage->setText(
			QString(rtspOutputHelper->GetLastError().c_str()));
	else
		ui->labelMessage->setText("");
}

void RtspProperties::showEvent(QShowEvent *event)
{
	UNUSED_PARAMETER(event);
	ui->checkBoxEnableMulticast->blockSignals(true);
	ui->checkBoxEnableMulticast->setChecked(
		obs_data_get_bool(settings, "multicast"));
	ui->checkBoxEnableMulticast->blockSignals(false);

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
	UNUSED_PARAMETER(event);
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
	UNUSED_PARAMETER(cd);
	auto page = static_cast<RtspProperties *>(data);
	page->setButtonStatus(false, true);
	page->setStatusTimerStatus(true);
}

void RtspProperties::OnOutputStop(void *data, calldata_t *cd)
{
	const auto page = static_cast<RtspProperties *>(data);
	if (const auto code = calldata_int(cd, "code");
	    code != OBS_OUTPUT_SUCCESS)
		page->setLabelMessageStatus(true);
	page->setButtonStatus(true, false);
	page->setStatusTimerStatus(false);
}

void RtspProperties::LoadConfig(config_t *config) const
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

void RtspProperties::SaveConfig(config_t *config) const
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

#endif // OBS_RTSPSERVER_ENABLE_UI
