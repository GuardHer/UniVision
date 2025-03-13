#pragma once

#include <ElaToggleButton.h>

class UniElaToggleButton : public ElaToggleButton
{
	Q_OBJECT
public:
	UniElaToggleButton(QWidget* parent = nullptr);
private:

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
};

