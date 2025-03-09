#include "UniCameraBase.h"

#include <QThread>

UniCameraBase::UniCameraBase(const CameraConfig& config, QObject* parent)
	: QObject(parent)
	, _config(config)
{
	_thread = new QThread(this);
	this->moveToThread(_thread);

	_thread->start();
}

UniCameraBase::~UniCameraBase()
{
	if (_thread)
	{
		_thread->quit();
		_thread->wait();
		_thread->deleteLater();
	}
}
