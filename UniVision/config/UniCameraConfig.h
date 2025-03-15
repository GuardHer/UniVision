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
		uint16_t _row;        // 行
		uint16_t _column;     // 列
		uint16_t _rowSpan;	  // 行跨度
		uint16_t _columnSpan; // 列跨度

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

	std::string _cameraIp;    // 相机IP
	std::string _cameraName;  // 相机名称
	std::string _cameraMark;  // 相机标记
	std::string _imagePath;   // 图像路径

	uint16_t _cameraIndex;    // 相机索引
	uint16_t _imageCount;     // 图像数量

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
		_imagePath = camera._imagePath;
		return *this;
	}

	bool operator==(const CameraConfig& camera) {
		return _cameraIp == camera._cameraIp &&
			_cameraType == camera._cameraType &&
			_cameraIndex == camera._cameraIndex &&
			_imageCount == camera._imageCount &&
			_cameraMark == camera._cameraMark &&
			_cameraLayout == camera._cameraLayout &&
			_imagePath == camera._imagePath &&
			_cameraName == camera._cameraName;
	}
};

