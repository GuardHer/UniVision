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
	_threadPool.setMaxThreadCount(config._threadNum);
}


UniAlgorithmManager::~UniAlgorithmManager()
{
	_threadPool.setMaxThreadCount(20);
	_threadPool.waitForDone(2000);
	_threadPool.deleteLater();
}

void UniAlgorithmManager::init()
{
	auto config = UNI_SETTINGS->getAlgorithmConfig();

	// 初始化算法
	g_pUniAlgorithm->init(config);
}

void UniAlgorithmManager::process(const AlgorithmInput& input)
{
	UniAlgorithProcess* process = new UniAlgorithProcess(input);
	process->setAutoDelete(true);
	connect(process, &UniAlgorithProcess::resultReady, this, &UniAlgorithmManager::resultReady, Qt::DirectConnection);
	_threadPool.start(process);
}


#pragma region UniAlgorithProcess
UniAlgorithProcess::UniAlgorithProcess(const AlgorithmInput& input)
	: _input(input)
{
}

UniAlgorithProcess::~UniAlgorithProcess()
{
}

void UniAlgorithProcess::run()
{
	AlgorithmOutput output;

	// start timeStamp
	auto start = std::chrono::high_resolution_clock::now();
	g_pUniAlgorithm->detect(_input, output);
	// end timeStamp
	auto end = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	LOG_INFO("算法耗时: {}, {}ms", output._imageMark, duration.count());

	Q_EMIT resultReady(_input, std::move(output));
}

#pragma endregion UniAlgorithProcess