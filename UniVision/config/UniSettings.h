#pragma once

#include <QObject>
#include <vector>
#include "config/UniCameraConfig.h"

class QSettings;

#define UNI_SETTINGS UniSettings::instance()

class UniSettings : public QObject
{
	Q_OBJECT
public:
	/// <summary>
	/// 获取单例
	/// </summary>
	/// <returns></returns>
	static UniSettings* instance();

	/// <summary>
	/// 获取settings
	/// </summary>
	/// <returns></returns>
	QSettings* settings() const;

	/// <summary>
	/// 获取相机配置
	/// </summary>
	/// <param name="config"></param>
	void getCameraConfigs(std::vector<CameraConfig>& config) const;

	/// <summary>
	/// 保存相机配置
	/// </summary>
	/// <param name="config"></param>
	void setCameraConfigs(const std::vector<CameraConfig>& config);

private:
	UniSettings(QObject* parent = nullptr);

	/// <summary>
	/// 不可copy 不可move
	/// </summary>
	Q_DISABLE_COPY_MOVE(UniSettings)

protected:

private:
	static UniSettings* _instance;

	QSettings* _settings = nullptr;
};

