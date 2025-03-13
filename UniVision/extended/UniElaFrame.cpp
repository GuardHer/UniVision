#include "UniElaFrame.h"

#include <QPainter>
#include <ElaApplication.h>

UniElaFrame::UniElaFrame(QWidget* parent)
	: QFrame(parent)
{
    // 主题
    _themeMode = eTheme->getThemeMode();
    connect(eTheme, &ElaTheme::themeModeChanged, this, [=](ElaThemeType::ThemeMode themeMode) {
        _themeMode = themeMode;
        update();
        });

    _isEnableMica = eApp->getIsEnableMica();
    connect(eApp, &ElaApplication::pIsEnableMicaChanged, this, [=]() {
        _isEnableMica = eApp->getIsEnableMica();
        update();
        });
    eApp->syncMica(this);
}

void UniElaFrame::paintEvent(QPaintEvent* event)
{
    if (!_isEnableMica)
    {
        QPainter painter(this);
        painter.save();
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(ElaThemeColor(_themeMode, WindowBase));
        painter.drawRect(rect());
        painter.restore();
    }
    QFrame::paintEvent(event);
}
