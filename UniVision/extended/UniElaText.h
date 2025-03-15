#pragma once

#include <QLabel>
#include <ElaTheme.h>
#include <ElaText.h>

class UniElaText : public ElaText
{
	Q_OBJECT
public:
	explicit UniElaText(QWidget* parent = nullptr);
	explicit UniElaText(const QString& text, QWidget* parent = nullptr);

public:
	void setBackColor(const QColor& color);
	void setBackColor(QColor&& color);
	QColor backColor() const;

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	QColor _backColor{ QColor(Qt::green) };
};

