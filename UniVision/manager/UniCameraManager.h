#pragma once

#include <mutex>
#include <unordered_map>
#include <memory>

#include "config/UniCameraConfig.h"
#include "device/camera/UniCameraBase.h"

#define g_pUniCameraManager UniCameraManager::instance()

class UniCameraBase;

class UniCameraManager
{
public:
	static UniCameraManager* instance();

	/// <summary>
	/// 生成相机key name-ip
	/// </summary>
	/// <param name="config"></param>
	/// <returns></returns>
	std::string generateCameraKey(const CameraConfig& config);

	/// <summary>
	/// 添加相机
	/// </summary>
	/// <param name="config"></param>
	/// <returns></returns>
	bool addCamera(const CameraConfig& config);

	/// <summary>
	/// 移除相机
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	bool removeCamera(const std::string& key);

	/// <summary>
	/// 打开相机
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	bool openCamera(const std::string& key);

	/// <summary>
	/// 关闭相机
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	bool closeCamera(const std::string& key);

	/// <summary>
	/// 开始采集
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	bool startGrabbing(const std::string& key);

	/// <summary>
	/// 停止采集
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	bool stopGrabbing(const std::string& key);

	/// <summary>
	/// 设置相机回调
	/// </summary>
	/// <param name="key"></param>
	/// <param name="callback"></param>
	void setGrabbingCallback(const std::string& key, const UniCameraBase::CameraCallback& callback);

	/// <summary>
	/// 获取相机指针
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	std::shared_ptr<UniCameraBase> getCameraPtr(const std::string& key);

	/// <summary>
	/// 获取相机连接状态
	/// </summary>
	/// <param name="mapConnect"></param>
	void getCamerasConnect(std::unordered_map<std::string /* cameraName */, bool>& mapConnect);

private:
	UniCameraManager();
	~UniCameraManager();

private:
	static std::mutex _mutex;
	static UniCameraManager* _instance;

	std::mutex _cameraMutex;
	std::unordered_map<std::string /* cameraIp */ , std::shared_ptr<UniCameraBase>> _cameraMap;
};

