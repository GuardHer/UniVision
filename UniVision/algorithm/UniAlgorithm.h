#pragma once

#include <mutex>
#include <vector>
#include <memory>
#include <deploy/model.hpp>
#include "config/UniAlgorithmConfig.h"

class ThreadPool;
class YoloSegOnnxDetect;

#define g_pUniAlgorithm UniAlgorithm::instance()
class UniAlgorithm
{

public:
	static UniAlgorithm* instance();

public:
	void setAlgorithmResultReadyCallback(const AlgorithmResultReadyCallback& callback);
	void detect(const AlgorithmInput& input, AlgorithmOutput& output);
	void init(const AlgorithmConfig& config);

private:
	std::vector<std::string> generate_labels(const std::string& label_file);

	UniAlgorithm();
	~UniAlgorithm();

private:
	static UniAlgorithm* _instance;
	static std::mutex _mutex;
	std::unique_ptr<deploy::SegmentModel> _modelTRT;
	std::unique_ptr<YoloSegOnnxDetect> _modelOnnx;


	std::mutex _mutexDetect;
	AlgorithmConfig _config;
	bool _isInit{ false };
	std::vector<cv::Scalar> _colors;
	AlgorithmResultReadyCallback _resultReadyCallback;
};

