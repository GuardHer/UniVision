#include "Template.h"
#include "UniAlgorithmManager.h"

#include "config/UniSettings.h"
#include "algorithm/UniAlgorithm.h"

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

	// 初始化算法
	g_pUniAlgorithm->init(std::move(config));
}


UniAlgorithmManager::~UniAlgorithmManager()
{
	_threadPool.setMaxThreadCount(20);
	_threadPool.waitForDone(2000);
	_threadPool.deleteLater();
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
	g_pUniAlgorithm->detect(_input, output);

	Q_EMIT resultReady(_input, std::move(output));
}

#pragma endregion UniAlgorithProcess