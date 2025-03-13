#pragma once

#include "extended/UniElaWidget.h"

class QGridLayout;
class UniImageViewFrame;

class UniMasterPage : public UniElaWidget
{
	Q_OBJECT
public:
	UniMasterPage(QWidget* parent = nullptr);

private:
	void initViews();

	void refreshViews();

protected:

public slots:

protected:
	QGridLayout* _gridLayout {nullptr};
};

