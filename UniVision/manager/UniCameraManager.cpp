#include "UniCameraManager.h"

#include "device/camera/UniSimCamera.h"

std::mutex UniCameraManager::_mutex;
UniCameraManager* UniCameraManager::_instance = nullptr;

UniCameraManager* UniCameraManager::instance()
{

	if (_instance == nullptr)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_instance == nullptr)
		{
			_instance = new UniCameraManager();
		}
	}

	return _instance;
}


UniCameraManager::UniCameraManager()
{
}

UniCameraManager::~UniCameraManager()
{
	std::lock_guard<std::mutex> lock(_cameraMutex);
	for (auto iter = _cameraMap.cbegin(); iter != _cameraMap.cend(); ++iter)
	{
		iter->second->stopGrabbing();
		iter->second->close();
	}
	_cameraMap.clear();
}


bool UniCameraManager::addCamera(const CameraConfig& config)
{
	std::string key = config._cameraIp;
	std::lock_guard<std::mutex> lock(_cameraMutex);
	if (_cameraMap.find(key) == _cameraMap.end())
	{
		std::shared_ptr<UniCameraBase> camera = nullptr;
		switch (config._cameraType)
		{
		case CameraType::UNI_SIMULATE_CAMERA:
			camera = std::make_shared<UniSimCamera>(config);
			break;
		case CameraType::UNI_BASLER_CAMERA:
			break;
		case CameraType::UNI_HIK_CAMERA:
			break;
		default:
			break;
		}
		if (camera != nullptr)
		{
			_cameraMap[key] = camera;
			return true;
		}
	}

	return false;
}

bool UniCameraManager::removeCamera(const std::string& key)
{
	std::lock_guard<std::mutex> lock(_cameraMutex);
	auto iter = _cameraMap.find(key);
	if (iter != _cameraMap.end())
	{
		_cameraMap.erase(iter);
		return true;
	}

	return false;
}

bool UniCameraManager::openCamera(const std::string& key)
{
	std::lock_guard<std::mutex> lock(_cameraMutex);
	auto iter = _cameraMap.find(key);
	if (iter != _cameraMap.end())
	{
		return iter->second->open();
	}

	return false;
}

bool UniCameraManager::closeCamera(const std::string& key)
{
	std::lock_guard<std::mutex> lock(_cameraMutex);
	auto iter = _cameraMap.find(key);
	if (iter != _cameraMap.end())
	{
		iter->second->close();
		return true;
	}

	return false;
}

bool UniCameraManager::startGrabbing(const std::string& key)
{
	std::lock_guard<std::mutex> lock(_cameraMutex);
	auto iter = _cameraMap.find(key);
	if (iter != _cameraMap.end())
	{
		return iter->second->startGrabbing();
	}
	return false;
}

bool UniCameraManager::stopGrabbing(const std::string& key)
{
	std::lock_guard<std::mutex> lock(_cameraMutex);
	auto iter = _cameraMap.find(key);
	if (iter != _cameraMap.end())
	{
		iter->second->stopGrabbing();
		return true;
	}
	return false;
}

std::shared_ptr<UniCameraBase> UniCameraManager::getCameraPtr(const std::string& key)
{
	std::lock_guard<std::mutex> lock(_cameraMutex);
	auto iter = _cameraMap.find(key);
	if (iter != _cameraMap.end())
	{
		return iter->second;
	}

	return nullptr;
}

void UniCameraManager::getCamerasConnect(std::unordered_map<std::string, bool>& mapConnect)
{
	std::lock_guard<std::mutex> lock(_cameraMutex);
	for (auto iter = _cameraMap.cbegin(); iter != _cameraMap.cend(); ++iter)
	{
		mapConnect[iter->second->getCameraName()] = iter->second->isConnected();
	}
}

