#pragma once

#include <mutex>
#include <vector>
#include <memory>
#include <deploy/model.hpp>
#include "config/UniAlgorithmConfig.h"

class ThreadPool;

#define g_pUniAlgorithm UniAlgorithm::instance()
class UniAlgorithm
{

public:
	static UniAlgorithm* instance();

public:
	void detect(const AlgorithmInput& input, AlgorithmOutput& output);
	void init(const AlgorithmConfig& config);

private:
	std::vector<std::string> generate_labels(const std::string& label_file);

	UniAlgorithm();
	~UniAlgorithm();

private:
	static UniAlgorithm* _instance;
	static std::mutex _mutex;
	std::shared_ptr<deploy::SegmentModel> _modelTRT;

	AlgorithmConfig _config;
	bool _isInit{ false };
};

