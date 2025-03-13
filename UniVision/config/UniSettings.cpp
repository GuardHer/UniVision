#include "UniSettings.h"

#include <QSettings>
#include <QApplication>

std::mutex UniSettings::_mutex;
UniSettings* UniSettings::_instance = nullptr;

UniSettings::UniSettings(QObject* parent)
	: QObject(parent)
{
	QString setting_file = qApp->applicationDirPath() + "/conf/settings.ini";
	_settings = new QSettings(setting_file, QSettings::IniFormat);

	// 注册类型 
	qRegisterMetaType<CameraType>("CameraType");

}

UniSettings* UniSettings::instance()
{
	if (_instance == nullptr) {
		std::lock_guard<std::mutex> lock(_mutex);
		if (_instance == nullptr) {
			_instance = new UniSettings();
		}
	}
	return _instance;
}

QSettings* UniSettings::settings() const
{
	return _settings;
}

void UniSettings::getCameraConfigs(std::vector<CameraConfig>& config) const
{
	if (!config.empty()) config.clear();

	int size = _settings->beginReadArray("CameraConfigs");
	for (int i = 0; i < size; ++i) {
		_settings->setArrayIndex(i);
		CameraConfig cfg;
		cfg._cameraIp = _settings->value("CameraIp").toString().toStdString();
		cfg._cameraName = _settings->value("CameraName").toString().toStdString();
		cfg._cameraMark = _settings->value("CameraMark").toString().toStdString();
		cfg._cameraType = _settings->value("CameraType").value<CameraType>();
		cfg._cameraIndex = _settings->value("CameraIndex").value<uint16_t>();
		cfg._imageCount = _settings->value("ImageCount").value<uint16_t>();
		cfg._cameraLayout._row = _settings->value("CameraLayout.row").value<uint16_t>();
		cfg._cameraLayout._column = _settings->value("CameraLayout.column").value<uint16_t>();
		cfg._cameraLayout._rowSpan = _settings->value("CameraLayout.rowSpan").value<uint16_t>();
		cfg._cameraLayout._columnSpan = _settings->value("CameraLayout.columnSpan").value<uint16_t>();

		config.push_back(std::move(cfg));
	}

	_settings->endArray();
}

void UniSettings::setCameraConfigs(const std::vector<CameraConfig>& config)
{
	_settings->beginWriteArray("CameraConfigs");
	for (int i = 0; i < config.size(); ++i) {
		_settings->setArrayIndex(i);
		_settings->setValue("CameraIp", QString::fromStdString(config[i]._cameraIp));
		_settings->setValue("CameraName", QString::fromStdString(config[i]._cameraName));
		_settings->setValue("CameraMark", QString::fromStdString(config[i]._cameraMark));
		_settings->setValue("CameraType", QVariant::fromValue(config[i]._cameraType));
		_settings->setValue("CameraIndex", QVariant::fromValue(config[i]._cameraIndex));
		_settings->setValue("ImageCount", QVariant::fromValue(config[i]._imageCount));
		_settings->setValue("CameraLayout.row", QVariant::fromValue(config[i]._cameraLayout._row));
		_settings->setValue("CameraLayout.column", QVariant::fromValue(config[i]._cameraLayout._column));
		_settings->setValue("CameraLayout.rowSpan", QVariant::fromValue(config[i]._cameraLayout._rowSpan));
		_settings->setValue("CameraLayout.columnSpan", QVariant::fromValue(config[i]._cameraLayout._columnSpan));
	}
	_settings->endArray();
	_settings->sync();
}
