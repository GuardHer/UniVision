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
	config.reserve(size);
	for (int i = 0; i < size; ++i) {
		_settings->setArrayIndex(i);
		CameraConfig cfg;
		cfg._cameraIp = _settings->value("CameraIp").toString().toStdString();
		cfg._cameraName = _settings->value("CameraName").toString().toStdString();
		cfg._cameraMark = _settings->value("CameraMark").toString().toStdString();
		cfg._imagePath = _settings->value("ImagePath").toString().toStdString();
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
		_settings->setValue("ImagePath", QString::fromStdString(config[i]._imagePath));
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

UniLogConfig UniSettings::getLogConfig() const
{
	UniLogConfig config;
	config._logName = _settings->value("Log/LogName", "UniLog").toString().toStdString();
	config._logSavePath = _settings->value("Log/LogSavePath", qApp->applicationDirPath() + "/Log/log.txt").toString().toStdString();
	config._logPattern = _settings->value("Log/LogPattern", "").toString().toStdString();
	config._logRotateHour = _settings->value("Log/LogRotateHour", 23).toInt();
	config._logRotateMinute = _settings->value("Log/LogRotateMinute", 59).toInt();
	config._level = _settings->value("Log/LogLevel", 1).toInt();
	return config;
}

void UniSettings::setLogConfig(const UniLogConfig& config)
{
	_settings->setValue("Log/LogName", QString::fromStdString(config._logName));
	_settings->setValue("Log/LogSavePath", QString::fromStdString(config._logSavePath));
	_settings->setValue("Log/LogPattern", QString::fromStdString(config._logPattern));
	_settings->setValue("Log/LogRotateHour", QVariant::fromValue(config._logRotateHour));
	_settings->setValue("Log/LogRotateMinute", QVariant::fromValue(config._logRotateMinute));
	_settings->setValue("Log/LogLevel", QVariant::fromValue(config._level));
	_settings->sync();
}

AlgorithmConfig UniSettings::getAlgorithmConfig() const
{
	AlgorithmConfig config;
	config._algorithmName = _settings->value("Algorithm/AlgorithmName", "Yolo").toString().toStdString();
	config._engineModelPath = _settings->value("Algorithm/EngineModelPath", qApp->applicationDirPath() + "/conf/Algorithm/model/model_yolo.engine").toString().toStdString();
	config._onnxModelPath = _settings->value("Algorithm/OnnxModelPath", qApp->applicationDirPath() + "/conf/Algorithm/model/model_yolo.onnx").toString().toStdString();
	config._labelPath = _settings->value("Algorithm/LabelPath", qApp->applicationDirPath() + "/conf/Algorithm/label/classes.txt").toString().toStdString();
	config._enableCuda = _settings->value("Algorithm/EnableCuda", true).toBool();
	config._gpuIndex = _settings->value("Algorithm/GpuIndex", 0).toInt();
	config._batchSize = _settings->value("Algorithm/BatchSize", 1).toInt();
	config._threadNum = _settings->value("Algorithm/ThreadNum", 6).toInt();
	config._detectType = static_cast<AlgorithmDetectType>(_settings->value("Algorithm/DetectType", 0).toInt());
	config._param._confidenceThreshold = _settings->value("Algorithm/Param.ConfidenceThreshold", 0.5).toFloat();
	config._param._nmsThreshold = _settings->value("Algorithm/Param.NmsThreshold", 0.4).toFloat();

	return config;
}

void UniSettings::setAlgorithmConfig(const AlgorithmConfig& config)
{
	_settings->setValue("Algorithm/AlgorithmName", QString::fromStdString(config._algorithmName));
	_settings->setValue("Algorithm/EngineModelPath", QString::fromStdString(config._engineModelPath));
	_settings->setValue("Algorithm/OnnxModelPath", QString::fromStdString(config._onnxModelPath));
	_settings->setValue("Algorithm/LabelPath", QString::fromStdString(config._labelPath));
	_settings->setValue("Algorithm/EnableCuda", QVariant::fromValue(config._enableCuda));
	_settings->setValue("Algorithm/GpuIndex", QVariant::fromValue(config._gpuIndex));
	_settings->setValue("Algorithm/BatchSize", QVariant::fromValue(config._batchSize));
	_settings->setValue("Algorithm/ThreadNum", QVariant::fromValue(config._threadNum));
	_settings->setValue("Algorithm/DetectType", QVariant::fromValue(static_cast<int>(config._detectType)));
	_settings->setValue("Algorithm/Param.ConfidenceThreshold", QVariant::fromValue(config._param._confidenceThreshold));
	_settings->setValue("Algorithm/Param.NmsThreshold", QVariant::fromValue(config._param._nmsThreshold));
	_settings->sync();
}
