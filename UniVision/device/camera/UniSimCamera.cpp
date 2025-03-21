#include "UniSimCamera.h"

#include <filesystem>

#include "core/UniLog.h"

UniSimCamera::UniSimCamera(const CameraConfig& config, QObject* parent)
	: UniCameraBase(config, parent)
{
	_isConnected = true;
}

UniSimCamera::~UniSimCamera()
{
	_isConnected = false;

	close();
}

bool UniSimCamera::open()
{
	_isOpened = true;

    return true;
}

void UniSimCamera::close()
{
	_isOpened = false;
}

bool UniSimCamera::startGrabbing()
{
    return true;
}

void UniSimCamera::stopGrabbing()
{
}

void UniSimCamera::setExposureTime(int time)
{
}

int UniSimCamera::getExposureTime()
{
	return 0;
}

void UniSimCamera::setTriggerSource(int source)
{
}

int UniSimCamera::getTriggerSource()
{
	return 0;
}

void UniSimCamera::setGain(int gain)
{
}

int UniSimCamera::getGain()
{
    return 0;
}

void UniSimCamera::setGamma(int gamma)
{
}

int UniSimCamera::getGamma()
{
    return 0;
}

void UniSimCamera::setTriggerMode(bool isOn)
{
}

bool UniSimCamera::getTriggerMode()
{
    return false;
}

void UniSimCamera::softwareTrigger()
{
	LOG_INFO("UniSimCamera::softwareTrigger {}", _config._cameraName);

	QMetaObject::invokeMethod(this, "onGrabbing", Qt::QueuedConnection);
}

void UniSimCamera::onGrabbing()
{
	if (!_isOpened) return;
	cv::Mat image = grabImage({ ".bmp", ".jpg", ".jpeg", ".png" }).clone();
	if (!image.empty()) {
		++_imageFrame;
		if (_callback) {
			_callback(image, _config._cameraMark, _config._cameraIndex, _imageFrame);
		}
	}
    else {
		LOG_WARN("UniSimCamera::onGrabbing: image is empty");
    }
}

cv::Mat UniSimCamera::grabImage(const std::set<std::string>& filter)
{
    cv::Mat image;

    if (filter != _lastFilter || _imagePaths.empty()) {
        updateImagePaths(filter);
    }

    if (_imagePaths.empty()) {
        return image;
    }

    image = cv::imread(_imagePaths[_currentIndex]);
    _currentIndex = (_currentIndex + 1) % _imagePaths.size();

    return image;
}

void UniSimCamera::updateImagePaths(const std::set<std::string>& filter)
{
    _imagePaths.clear();
    _currentIndex = 0;
    _lastFilter = filter;

    try 
    {
        auto path = std::filesystem::path(_config._imagePath);
        if (!std::filesystem::exists(path) ||
            !std::filesystem::is_directory(path)) {
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(_config._imagePath)) {
            std::string ext = entry.path().extension().string();
            if (filter.count(ext)) {
                _imagePaths.push_back(entry.path().string());
            }
        }

        std::sort(_imagePaths.begin(), _imagePaths.end());
    }
    catch (const std::exception& e) 
    {
        LOG_ERROR("UniSimCamera::updateImagePaths: {}", e.what());
    }
	
}

