#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <unordered_map>
#include <functional>

#include <opencv2/opencv.hpp>

#include "core/UniLog.h"

enum class AlgorithmDetectType : int
{
	YOLO_TRT_SEG,
	YOLO_ONNX_SEG,
    PURE_ONNX_SEG,
};

inline std::string AlgorithmDetectTypeToString(AlgorithmDetectType type)
{
	switch (type)
	{
	case AlgorithmDetectType::YOLO_TRT_SEG:
		return "YOLO-TensorRT";
	case AlgorithmDetectType::YOLO_ONNX_SEG:
		return "YOLO-ONNX";
	case AlgorithmDetectType::PURE_ONNX_SEG:
		return "PURE_ONNX";
	default:
		return "Unknown";
	}
}


inline std::vector<std::string> AlgorithmDetectTypeToStrings()
{
	std::vector<std::string> list;
	list.push_back(AlgorithmDetectTypeToString(AlgorithmDetectType::YOLO_TRT_SEG));
	list.push_back(AlgorithmDetectTypeToString(AlgorithmDetectType::YOLO_ONNX_SEG));
	list.push_back(AlgorithmDetectTypeToString(AlgorithmDetectType::PURE_ONNX_SEG));
	
	return list;
}


inline AlgorithmDetectType StringToAlgorithmDetectType(const std::string& str)
{
	if (str == "YOLO-TensorRT")
	{
		return AlgorithmDetectType::YOLO_TRT_SEG;
	}
	else if (str == "YOLO-ONNX")
	{
		return AlgorithmDetectType::YOLO_ONNX_SEG;
	}
	else if (str == "PURE_ONNX")
	{
		return AlgorithmDetectType::PURE_ONNX_SEG;
	}
	else
	{
		return AlgorithmDetectType::YOLO_TRT_SEG;
	}
}

template <typename T>
inline std::string VectorToString(const std::vector<T>& vec)
{
	std::stringstream ss;
	for (size_t i = 0; i < vec.size(); ++i)
	{
		ss << vec[i];
		if (i < vec.size() - 1)
		{
			ss << ",";
		}
	}
	return ss.str();
}


template<typename T>
inline std::vector<T> StringToVector(const std::string& str)
{
	if (str.empty())
	{
		return std::vector<T>();
	}

	// int 
	if constexpr (std::is_same_v<T, int>)
	{
		std::vector<T> vec;
		std::stringstream ss(str);
		std::string item;
		while (std::getline(ss, item, ','))
		{
			vec.push_back(std::stoi(item));
		}
		return vec;
	}

	// float
	if constexpr (std::is_same_v<T, float>)
	{
		std::vector<T> vec;
		std::stringstream ss(str);
		std::string item;
		while (std::getline(ss, item, ','))
		{
			vec.push_back(std::stof(item));
		}
		return vec;
	}

	// double
	if constexpr (std::is_same_v<T, double>)
	{
		std::vector<T> vec;
		std::stringstream ss(str);
		std::string item;
		while (std::getline(ss, item, ','))
		{
			vec.push_back(std::stod(item));
		}
		return vec;
	}

	// std::string
    if constexpr (std::is_same_v<T, std::string>) {
            std::vector<T> vec;
            std::stringstream ss(str);
            std::string item;
            while (std::getline(ss, item, ',')) {
                    vec.push_back(item);
            }
            return vec;
    }
}



struct AlgotithmParam
{
	float _confidenceThreshold{ 0.5f }; // 置信度阈值
	float _nmsThreshold{ 0.4f };        // NMS阈值

	std::vector<float> _meanValues;     // 均值
	std::vector<float> _stdValues;      // 标准差
	std::vector<int> _minArea;      // 最小面积
};

struct AlgorithmConfig
{
	std::string _algorithmName; // 算法名称
	std::string _modelPath;     // 模型路径
	std::string _modelName;     // 模型名称
	std::string _labelPath;     // 标签路径
	bool _enableCuda{ true };   // 是否启用 CUDA
	size_t _gpuIndex{ 0 };      // GPU索引
	size_t _threadNum{ 6 };     // 线程数量
	AlgorithmDetectType _detectType; // 算法类型

	AlgotithmParam _param;      // 算法参数

	// operate =
	AlgorithmConfig& operator=(const AlgorithmConfig& other)
	{
		_algorithmName = other._algorithmName;
		_modelPath = other._modelPath;
		_modelName = other._modelName;
		_labelPath = other._labelPath;
		_enableCuda = other._enableCuda;
		_gpuIndex = other._gpuIndex;
		_threadNum = other._threadNum;
		_detectType = other._detectType;
		_param = other._param;
		return *this;
	}

	std::vector<std::string> getVecModelName() const
	{
		std::vector<std::string> vecModelName;

		if (_modelName.empty()) {
			return vecModelName;
		}

		std::stringstream ss(_modelName);
		std::string filename;

		// 分割逗号分隔的字符串
		while (std::getline(ss, filename, ',')) {
			filename.erase(0, filename.find_first_not_of(" \t"));
			filename.erase(filename.find_last_not_of(" \t") + 1);

			if (filename.empty()) {
				continue;
			}

			try {
				std::string fullPath = (std::filesystem::path(_modelPath) / filename).string();
				vecModelName.push_back(fullPath);
			}
			catch (const std::filesystem::filesystem_error& e) {
				LOG_ERROR("Failed to get model name: {}", e.what());
				continue; 
			}
		}

		// 如果没有有效文件名，检查是否 _modelName 本身是一个文件名
		if (vecModelName.empty() && !_modelName.empty()) {
			try {
				std::string fullPath = (std::filesystem::path(_modelPath) / _modelName).string();
				vecModelName.push_back(fullPath);
			}
			catch (const std::filesystem::filesystem_error& e) {
				LOG_ERROR("Failed to get model name: {}", e.what());
			}
		}

		return vecModelName;
	}
};


struct AlgorithmInput
{
	cv::Mat _srcImage;            // 输入图像
	uint16_t _cameraIndex;        // 相机索引
	uint16_t _imageIndex;         // 图像索引
	std::string _imageMark;       // 图像标记
	bool _forceUseCpu{ false };   // 是否强制使用CPU

};

struct AlgorithmOutput
{
	struct Result
	{
		std::string _class; // 标签
		float _score;       // 分数
		cv::Mat _mask;      // 分割掩码 (Seg)
		cv::Rect _rect;     // 矩形框
	};

	cv::Mat _dstImage;            // 输出效果图像
	std::string _imageMark;       // 图像标记
	uint16_t _cameraIndex;        // 相机索引
	uint16_t _imageIndex;         // 图像索引
	bool _bResult;                // 是否有结果
	std::vector<Result> _results; // 检测结果
};

using AlgorithmResultReadyCallback = std::function<void(const AlgorithmInput&, const AlgorithmOutput&)>;