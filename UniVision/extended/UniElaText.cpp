#include "UniElaText.h"

#include <QPainter>
#include <ElaApplication.h>

UniElaText::UniElaText(QWidget* parent)
	: ElaText(parent)
{
}

UniElaText::UniElaText(const QString& text, QWidget* parent)
	: ElaText(text, parent)
{
}

void UniElaText::setBackColor(const QColor& color)
{
	_backColor = color;
	update();
}

void UniElaText::setBackColor(QColor&& color)
{
	_backColor = std::move(color);
	update();
}

QColor UniElaText::backColor() const
{
	return _backColor;
}

void UniElaText::paintEvent(QPaintEvent* event)
{
	// 绘制背景
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(Qt::NoPen);
	painter.setBrush(_backColor);
	QRect rect = this->rect();
	painter.drawRect(rect.x(), rect.y(), rect.width() - 10, rect.height());

	ElaText::paintEvent(event);
}
