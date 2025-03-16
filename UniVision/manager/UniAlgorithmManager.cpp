#include "Template.h"
#include "UniAlgorithmManager.h"

#include "config/UniSettings.h"
#include "algorithm/UniAlgorithm.h"
#include "core/UniLog.h"

UniAlgorithmManager* UniAlgorithmManager::_instance = nullptr;
std::mutex UniAlgorithmManager::_mutex;

UniAlgorithmManager* UniAlgorithmManager::instance()
{
	if (_instance == nullptr)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_instance == nullptr)
		{
			_instance = new UniAlgorithmManager();
		}
	}
	return _instance;
}

UniAlgorithmManager::UniAlgorithmManager()
{
	auto config = UNI_SETTINGS->getAlgorithmConfig();
	//_threadPool.setMaxThreadCount(config._threadNum);

	_uniThreadPoolPtr = std::make_unique<UniThreadPool>(config._threadNum);
}


UniAlgorithmManager::~UniAlgorithmManager()
{
}

void UniAlgorithmManager::init()
{
	auto config = UNI_SETTINGS->getAlgorithmConfig();

	// 初始化算法
	g_pUniAlgorithm->init(config);
}

void UniAlgorithmManager::process(const AlgorithmInput& input)
{
	auto AlgoExec = std::bind(&UniAlgorithmManager::execute, this, input);
	_uniThreadPoolPtr->enqueue(std::move(AlgoExec));
}

void UniAlgorithmManager::execute(const AlgorithmInput& input)
{
	AlgorithmOutput output;

	// start timeStamp
	auto start = std::chrono::high_resolution_clock::now();

	try
	{
		g_pUniAlgorithm->detect(input, output);
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("算法异常: {}", e.what());
	}
	catch (...)
	{
		LOG_ERROR("算法异常: 未知异常");
	}

	// end timeStamp
	auto end = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	LOG_INFO("算法耗时: {}, {}ms", output._imageMark, duration.count());
}

