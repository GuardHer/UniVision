#include "UniSimCamera.h"

#include <filesystem>

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

void UniSimCamera::open()
{
	_isOpened = true;
}

void UniSimCamera::close()
{
	_isOpened = false;
}

void UniSimCamera::startGrabbing()
{

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

void UniSimCamera::softwareTrigger()
{
	QMetaObject::invokeMethod(this, "onGrabbing", Qt::QueuedConnection);
}

void UniSimCamera::onGrabbing()
{
	if (!_isOpened) return;
	cv::Mat image = grabImage({ ".bmp", ".jpg", ".png" }).clone();
	if (!image.empty()) {
		++_imageFrame;
		if (_callback) {
			_callback(image, _config._cameraMark, _config._cameraIndex, _imageFrame);
		}
	}
    else {
		std::cout << "No image grabbed!" << std::endl;
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

    for (const auto& entry : std::filesystem::directory_iterator(_imagePath)) {
        std::string ext = entry.path().extension().string();
        if (filter.count(ext)) {
            _imagePaths.push_back(entry.path().string());
        }
    }

    std::sort(_imagePaths.begin(), _imagePaths.end());
}

