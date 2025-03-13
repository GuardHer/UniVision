#pragma once

#include <QObject>
#include <atomic>
#include <memory>
#include <opencv2/opencv.hpp>

#include "config/UniCameraConfig.h"

class QThread;
class UniCameraBase : public QObject
{
	Q_OBJECT

public:
	using s_ptr = std::shared_ptr<UniCameraBase>;
	using u_ptr = std::unique_ptr<UniCameraBase>;

	using CameraCallback = std::function<void(const cv::Mat& srcImg, 
		const std::string& imageMark, 
		uint16_t cameraIndex, 
		uint16_t imageIndex)>;

	enum TriggerSource { Software, Hardware };
	Q_ENUM(TriggerSource)

public:
	UniCameraBase(const CameraConfig& config, QObject* parent = nullptr);
	virtual ~UniCameraBase();

	/// <summary>
	/// open camera
	/// </summary>
	virtual bool open() = 0;

	/// <summary>
	/// close camera
	/// </summary>
	virtual void close() = 0;

	/// <summary>
	/// 开始采集
	/// </summary>
	virtual bool startGrabbing() = 0;

	/// <summary>
	/// 停止采集
	/// </summary>
	virtual void stopGrabbing() = 0;

	/// <summary>
	/// 设置曝光时间
	/// </summary>
	/// <param name="time"></param>
	virtual void setExposureTime(int time) = 0;

	/// <summary>
	/// 获取曝光时间
	/// </summary>
	/// <returns></returns>
	virtual int getExposureTime() = 0;

	/// <summary>
	/// 设置增益
	/// </summary>
	/// <param name="gain"></param>
	virtual void setGain(int gain) = 0;

	/// <summary>
	/// 获取增益
	/// </summary>
	/// <returns></returns>
	virtual int getGain() = 0;

	/// <summary>
	/// 设置伽马
	/// </summary>
	/// <param name="gamma"></param>
	virtual void setGamma(int gamma) = 0;

	/// <summary>
	/// 获取伽马
	/// </summary>
	/// <returns></returns>
	virtual int getGamma() = 0;

	/// <summary>
	/// 设置触发模式
	/// </summary>
	/// <param name="isOn"></param>
	virtual void setTriggerMode(bool isOn) = 0;

	/// <summary>
	/// 获取触发模式
	/// </summary>
	/// <returns></returns>
	virtual bool getTriggerMode() = 0;

	/// <summary>
	/// 设置触发源
	/// </summary>
	/// <param name="source"></param>
	virtual void setTriggerSource(int source) = 0;

	/// <summary>
	/// 获取触发源
	/// </summary>
	/// <returns></returns>
	virtual int getTriggerSource() = 0;

	/// <summary>
	/// 软触发
	/// </summary>
	virtual void softwareTrigger() = 0;

public:
	/// <summary>
	/// 相机是否打开
	/// </summary>
	/// <returns></returns>
	bool isOpened() const { return _isOpened; }

	/// <summary>
	/// 相机是否连接
	/// </summary>
	/// <returns></returns>
	bool isConnected() const { return _isConnected; }

	/// <summary>
	/// 设置回调
	/// </summary>
	/// <param name="callback"></param>
	void setCallback(const CameraCallback& callback) { _callback = callback; }
	void setCallback(CameraCallback&& callback) { _callback = std::move(callback); }

	/// <summary>
	/// 获取当前帧数
	/// </summary>
	/// <returns></returns>
	int32_t getImageFrame() { return _imageFrame; }

	/// <summary>
	/// 重置帧数
	/// </summary>
	void resetImageFrame() { _imageFrame = 0; }

	/// <summary>
	/// 获取相机IP
	/// </summary>
	/// <returns></returns>
	std::string getCameraIpAddress() const { return _config._cameraIp; }

	/// <summary>
	/// 获取相机名称
	/// </summary>
	/// <returns></returns>
	std::string getCameraName() const { return _config._cameraName; }

protected:
	QThread* _thread {nullptr};
	CameraConfig _config;
	std::atomic_bool _isOpened{ false };
	std::atomic_bool _isConnected{ false };
	std::atomic_int32_t _imageFrame{ 0 };

	CameraCallback _callback;
};

