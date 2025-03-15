#pragma once

#include <mutex>
#include <QObject>
#include <QRunnable>
#include <QThreadPool>

#include "config/UniAlgorithmConfig.h"

#define g_pUniAlgorithmManager UniAlgorithmManager::instance()

class UniAlgorithProcess : public QObject, public QRunnable
{
	Q_OBJECT
public:
	UniAlgorithProcess(const AlgorithmInput& input);
	~UniAlgorithProcess();

Q_SIGNALS:
	void resultReady(const AlgorithmInput& input, const AlgorithmOutput& output);


protected:
	void run() override;

private:
	AlgorithmInput _input;
};

class UniAlgorithmManager : public QObject
{
	Q_OBJECT
public:
	static UniAlgorithmManager* instance();

private:
	UniAlgorithmManager();
	~UniAlgorithmManager();

	Q_DISABLE_COPY_MOVE(UniAlgorithmManager)

public:
	void init();

	void process(const AlgorithmInput& input);

Q_SIGNALS:
	void resultReady(const AlgorithmInput& input, const AlgorithmOutput& output);

private:

private:
	static UniAlgorithmManager* _instance;
	static std::mutex _mutex;

	QThreadPool _threadPool;
};

