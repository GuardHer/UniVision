#pragma once

#include <mutex>
#include <QObject>
#include <QRunnable>
#include <QThreadPool>

#include "config/UniAlgorithmConfig.h"
#include "core/UniThreadPool.h"

#define g_pUniAlgorithmManager UniAlgorithmManager::instance()

class UniAlgorithmManager
{
public:
	static UniAlgorithmManager* instance();

private:
	UniAlgorithmManager();
	~UniAlgorithmManager();

	Q_DISABLE_COPY_MOVE(UniAlgorithmManager)

public:
	void init();

	void process(const AlgorithmInput& input);

private:
	void execute(const AlgorithmInput& input);

private:

private:
	static UniAlgorithmManager* _instance;
	static std::mutex _mutex;

	std::unique_ptr<UniThreadPool> _uniThreadPoolPtr;
};

