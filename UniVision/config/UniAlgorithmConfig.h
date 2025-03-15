#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <opencv2/opencv.hpp>

enum class AlgorithmDetectType : int
{
	UNI_ALGORITHM_TOLO_SEG,
	UNI_ALGORITHM_MMCV_SEG,
};

struct AlgotithmParam
{
	float _confidenceThreshold{ 0.5f }; // 置信度阈值
	float _nmsThreshold{ 0.4f };        // NMS阈值
};

struct AlgorithmConfig
{
	std::string _algorithmName; // 算法名称
	std::string _engineModelPath;     // 模型路径
	std::string _onnxModelPath;     // 模型路径
	std::string _labelPath;     // 标签路径
	bool _enableCuda{ true };    // 是否启用CUDA
	size_t _gpuIndex{ 0 };       // GPU索引
	size_t _batchSize{ 1 };      // 批处理大小
	size_t _threadNum{ 6 };      // 线程数量
	AlgorithmDetectType _detectType; // 算法类型

	AlgotithmParam _param;      // 算法参数

	// operate =
	AlgorithmConfig& operator=(const AlgorithmConfig& other)
	{
		_algorithmName = other._algorithmName;
		_engineModelPath = other._engineModelPath;
		_onnxModelPath = other._onnxModelPath;
		_labelPath = other._labelPath;
		_enableCuda = other._enableCuda;
		_gpuIndex = other._gpuIndex;
		_batchSize = other._batchSize;
		_threadNum = other._threadNum;
		_detectType = other._detectType;
		_param = other._param;
		return *this;
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
