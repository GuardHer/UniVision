#pragma once

#include <unordered_map>

#include "extended/UniElaWidget.h"
#include "config/UniAlgorithmConfig.h"

class QGridLayout;
class UniImageViewFrame;

class UniMasterPage : public UniElaWidget
{
	Q_OBJECT
public:
	UniMasterPage(QWidget* parent = nullptr);
	void showImage(const cv::Mat& image, const std::string& mark);

private:
	void initViews();
	void refreshViews();


protected:

public slots:

protected:
	QGridLayout* _gridLayout {nullptr};
	std::unordered_map<std::string, UniImageViewFrame*> _imageViews;
};

