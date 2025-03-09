#pragma once

#include <string>

enum class CameraType
{
	UNI_SIMULATE_CAMERA, // 模拟相机
	UNI_BASLER_CAMERA,   // basler相机
	UNI_HIK_CAMERA,      // 海康相机
};


struct CameraConfig
{
	struct CameraLayout
	{
		uint16_t _row;
		uint16_t _column;
		uint16_t _rowSpan;
		uint16_t _columnSpan;

		CameraLayout operator=(const CameraLayout& layout) {
			_row = layout._row;
			_column = layout._column;
			_rowSpan = layout._rowSpan;
			_columnSpan = layout._columnSpan;
			return *this;
		}

		bool operator==(const CameraLayout& layout) {
			return _row == layout._row &&
				_column == layout._column &&
				_rowSpan == layout._rowSpan &&
				_columnSpan == layout._columnSpan;
		}
	};

	std::string _cameraIp;
	std::string _cameraName;
	std::string _cameraMark;

	uint16_t _cameraIndex;
	uint16_t _imageCount;

	CameraType _cameraType;
	CameraLayout _cameraLayout;

	CameraConfig operator=(const CameraConfig& camera) {
		_cameraIp = camera._cameraIp;
		_cameraType = camera._cameraType;
		_cameraIndex = camera._cameraIndex;
		_imageCount = camera._imageCount;
		_cameraMark = camera._cameraMark;
		_cameraLayout = camera._cameraLayout;
		_cameraName = camera._cameraName;
		return *this;
	}

	bool operator==(const CameraConfig& camera) {
		return _cameraIp == camera._cameraIp &&
			_cameraType == camera._cameraType &&
			_cameraIndex == camera._cameraIndex &&
			_imageCount == camera._imageCount &&
			_cameraMark == camera._cameraMark &&
			_cameraLayout == camera._cameraLayout &&
			_cameraName == camera._cameraName;
	}
};

