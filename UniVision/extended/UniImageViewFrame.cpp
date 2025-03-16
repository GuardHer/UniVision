#include "UniImageViewFrame.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsItem>
#include <ElaText.h>
#include <ElaGraphicsView.h>
#include <ElaGraphicsScene.h>
#include <ElaTheme.h>
#include <QOpenGLWidget>
#include <QDebug>
#include <QTimer>

class UniImageViewFramePrivate
{
	friend class UniImageViewFrame;

public:
	UniImageViewFramePrivate(UniImageViewFrame* q)
		: q_ptr(q)
	{
		_view = new ElaGraphicsView(q);
		_scene = new ElaGraphicsScene(q);
		//_view->setViewport(new QOpenGLWidget(_view));
		_view->setScene(_scene);

		//_text = new ElaText(q);
		//_text->setTextPointSize(12);
	}

	~UniImageViewFramePrivate()
	{
		if (_view) {
			_view->deleteLater();
		}
		if (_scene) {
			_scene->deleteLater();
		}
		if (_text) {
			_text->deleteLater();
		}
	}

private:
	UniImageViewFrame* q_ptr = nullptr;
	ElaGraphicsView* _view = nullptr;
	ElaGraphicsScene* _scene = nullptr;
	QGraphicsItem* _currItem = nullptr;
	ElaText* _text = nullptr;
};

UniImageViewFrame::UniImageViewFrame(const std::string& key, QWidget* parent)
	: UniElaFrame(parent)
	, _key(key)
	, d(new UniImageViewFramePrivate(this))
{
	if (d->_text) {
		d->_text->setText(QString::fromStdString(key));
	}

	connect(eTheme, &ElaTheme::themeModeChanged, this, [this](ElaThemeType::ThemeMode theme) {
		if (d->_view)
		{
			QColor color = theme == ElaThemeType::ThemeMode::Dark
				? QColor(0, 0, 0)
				: QColor(255, 255, 255);

			d->_view->setBackgroundBrush(color);
		}
	});
}

ElaGraphicsView* UniImageViewFrame::view() const
{
	return d->_view;
}

std::string UniImageViewFrame::key() const
{
	return _key;
}

void UniImageViewFrame::setImage(const QImage& image)
{
	if (image.isNull()) return;

	d->_scene->clear();
	delete d->_currItem;
	d->_currItem = nullptr;
	d->_currItem = d->_scene->addPixmap(QPixmap::fromImage(image));
	dynamic_cast<QGraphicsPixmapItem*>(d->_currItem)->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
	
	d->_scene->setSceneRect(d->_currItem->boundingRect());
	d->_view->resetTransform();

	if (d->_currItem) {
		d->_view->fitInView(d->_currItem, Qt::KeepAspectRatio);
	}
}

void UniImageViewFrame::setImage(const cv::Mat& image)
{
	if (image.empty()) return;
	setImage(cvMat2QImage(image));
}

void UniImageViewFrame::resizeEvent(QResizeEvent* event)
{
	if (d->_view) {
		d->_view->setFixedSize(width(), height());

		if (d->_currItem) {
			d->_view->fitInView(d->_currItem, Qt::KeepAspectRatio);
		}
	}

	if (d->_text) {
		QString text = d->_text->text();
		QFontMetrics fm(d->_text->font());
		int textWidth = fm.horizontalAdvance(text);

		int margin = d->_text->margin() * 2;     // 左右 margin 各一次
		int indent = d->_text->indent() * 2;     // 左右 indent 各一次
		int totalWidth = textWidth + margin + indent;
		int pos = (width() - totalWidth) / 2;

		d->_text->setGeometry(pos, 5, d->_text->width(), d->_text->height());
	}

	QFrame::resizeEvent(event);
}

//void UniImageViewFrame::paintEvent(QPaintEvent* event)
//{
//	if (d->_view)
//	{
//		QColor color = eTheme->getThemeMode() == ElaThemeType::ThemeMode::Dark 
//			? QColor(0, 0, 0) 
//			: QColor(255, 255, 255);
//
//		d->_view->setBackgroundBrush(color);
//	}
//
//	QFrame::paintEvent(event);
//}



QImage UniImageViewFrame::cvMat2QImage(const cv::Mat& mat)
{
	QImage image;
	switch (mat.type())
	{
	case CV_8UC1:
		image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
		break;
	case CV_8UC3:
		image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.cols * 3, QImage::Format_BGR888);
		break;
	case CV_8UC4:
		image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		break;
	case CV_16UC4:
		image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA64);
		image = image.rgbSwapped();
		break;
	}
	return image;
}
