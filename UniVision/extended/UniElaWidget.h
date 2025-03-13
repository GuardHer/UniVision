#pragma once

#include <QWidget>
#include <ElaTheme.h>

class UniElaWidget : public QWidget
{
	Q_OBJECT
public:
	UniElaWidget(QWidget* parent = nullptr);

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	ElaThemeType::ThemeMode _themeMode;
	bool _isEnableMica;
};

