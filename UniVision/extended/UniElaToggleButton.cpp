#include "UniElaToggleButton.h"

#include <QMouseEvent>

UniElaToggleButton::UniElaToggleButton(QWidget* parent)
	: ElaToggleButton(parent)
{
}

void UniElaToggleButton::mousePressEvent(QMouseEvent* event)
{
	QWidget::mousePressEvent(event);
}

void UniElaToggleButton::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);
}
