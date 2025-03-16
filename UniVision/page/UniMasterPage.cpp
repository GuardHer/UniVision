#include "UniMasterPage.h"

#include <QGridLayout>
#include <QDragEnterEvent>
#include <QApplication>
#include <QToolBar>
#include <QSpinBox>
#include <QLabel>
#include <ElaGraphicsView.h>
#include <ElaGraphicsScene.h>

#include "config/UniSettings.h"
#include "extended/UniImageViewFrame.h"

UniMasterPage::UniMasterPage(QWidget* parent)
	: UniElaWidget(parent)
{
    _gridLayout = new QGridLayout(this);

	initViews();
}

void UniMasterPage::initViews()
{
	std::vector<CameraConfig> configs;
	UNI_SETTINGS->getCameraConfigs(configs);

	for (int i = 0; i < configs.size(); i++)
	{
		UniImageViewFrame* frame = new UniImageViewFrame(configs[i]._cameraMark, this);
		_imageViews.insert(std::make_pair(configs[i]._cameraMark, frame));
		//frame->setImage(QImage("C:\\Users\\gshtang\\Desktop\\shixun\\start.bmp"));
		_gridLayout->addWidget(frame, configs[i]._cameraLayout._row, configs[i]._cameraLayout._column
		                      , configs[i]._cameraLayout._rowSpan, configs[i]._cameraLayout._columnSpan);
	}
}

void UniMasterPage::refreshViews()
{
	while (_gridLayout->count() > 0)
	{
		QLayoutItem* item = _gridLayout->takeAt(0);
		if (item->widget())
		{
			item->widget()->deleteLater();
		}
		delete item;
		item = nullptr;
	}
	_imageViews.clear();
	initViews();
}

void UniMasterPage::showImage(const AlgorithmOutput& output)
{
	QMetaObject::invokeMethod(this, "showImage", Qt::QueuedConnection, Q_ARG(cv::Mat, output._dstImage), Q_ARG(std::string, output._imageMark));
}

void UniMasterPage::showImage(const cv::Mat& image, const std::string& mark)
{
	auto it = _imageViews.find(mark);
	if (it != _imageViews.end())
	{
		if (it->second)
			it->second->setImage(image);
	}
}

