#pragma once

#include "device/camera/UniCameraBase.h"

class UniSimCamera : public UniCameraBase
{
	Q_OBJECT

public:
	UniSimCamera(const CameraConfig& config, QObject* parent = nullptr);
	virtual ~UniSimCamera();

public:
	/// <summary>
	/// 设置模拟相机采图路径
	/// </summary>
	/// <param name="path"></param>
	void setImagePath(const std::string& path) { _imagePath = path; }

protected:
	/// <summary>
	/// open camera
	/// </summary>
	virtual void open() override;
	/// <summary>
	/// close camera
	/// </summary>
	virtual void close() override;
	/// <summary>
	/// 开始采集
	/// </summary>
	virtual void startGrabbing() override;
	/// <summary>
	/// 停止采集
	/// </summary>
	virtual void stopGrabbing() override;
	/// <summary>
	/// 设置曝光时间
	/// </summary>
	/// <param name="time"></param>
	virtual void setExposureTime(int time) override;
	/// <summary>
	/// 获取曝光时间
	/// </summary>
	/// <returns></returns>
	virtual int getExposureTime() override;
	/// <summary>
	/// 设置触发源
	/// </summary>
	/// <param name="source"></param>
	virtual void setTriggerSource(int source) override;
	/// <summary>
	/// 获取触发源
	/// </summary>
	/// <returns></returns>
	virtual int getTriggerSource() override;
	/// <summary>
	/// 软触发
	/// </summary>
	virtual void softwareTrigger() override;

private slots:
	void onGrabbing();

private:
	/// <summary>
	/// 从文件夹中读取图片
	/// </summary>
	/// <param name="filter"> 文件过滤 </param>
	cv::Mat grabImage(const std::set<std::string>& filter);

	/// <summary>
	/// 更新图片路径
	/// </summary>
	/// <param name="filter"></param>
	void updateImagePaths(const std::set<std::string>& filter);

private:
	std::string _imagePath;                 // 图片目录路径
	std::vector<std::string> _imagePaths;   // 缓存符合条件的文件路径
	size_t _currentIndex = 0;               // 当前文件索引
	std::set<std::string> _lastFilter;      // 上一次使用的文件过滤器
};

