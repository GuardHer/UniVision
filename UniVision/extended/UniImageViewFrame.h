#pragma once

#include <string>
#include <opencv2/opencv.hpp>

#include "extended/UniElaFrame.h"
#include "extended/UniElaWidget.h"

class ElaGraphicsView;
class ElaGraphicsScene;
class UniImageViewFramePrivate;

class UniImageViewFrame : public UniElaFrame
{
	Q_OBJECT
public:
	explicit UniImageViewFrame(const std::string& key, QWidget* parent = nullptr);

	/// <summary>
	/// 获取视图
	/// </summary>
	/// <returns></returns>
	ElaGraphicsView* view() const;

	/// <summary>
	/// 获取显示name
	/// </summary>
	/// <returns></returns>
	std::string key() const;

	/// <summary>
	/// 更新图片
	/// </summary>
	/// <param name="image"></param>
	void setImage(const QImage& image);
	void setImage(const cv::Mat& image);

private:

protected:
	void resizeEvent(QResizeEvent*) override;
	void paintEvent(QPaintEvent* event) override;


private:
	std::string _key;

	UniImageViewFramePrivate* d = nullptr;
};

