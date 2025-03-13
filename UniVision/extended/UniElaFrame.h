#pragma once

#include <QFrame>
#include <ElaTheme.h>

class UniElaFrame : public QFrame
{
	Q_OBJECT
public:
	UniElaFrame(QWidget* parent = nullptr);

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	ElaThemeType::ThemeMode _themeMode;
	bool _isEnableMica;
};

