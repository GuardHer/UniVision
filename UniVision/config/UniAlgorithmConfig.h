#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <opencv2/opencv.hpp>

enum class AlgorithmDetectType
{
	UNI_ALGORITHM_CLS,
	UNI_ALGORITHM_SEG,
	UNI_ALGORITHM_OBB,
};

struct AlgotithmParam
{
	float _confidenceThreshold{ 0.5f }; // 置信度阈值
	float _nmsThreshold{ 0.4f };        // NMS阈值
	cv::Rect _roi;                      // 检测区域
};

struct AlgorithmConfig
{
	std::string _algorithmName; // 算法名称
	std::string _modelPath;     // 模型路径
	std::string _labelPath;     // 标签路径
	AlgorithmDetectType _detectType; // 算法类型

	AlgotithmParam _param;      // 算法参数
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

	std::vector<Result> _results; // 检测结果
};