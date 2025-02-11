#pragma once

#include <QtWidgets/QMainWindow>
#include <ElaWindow.h>

class UniVision : public ElaWindow
{
    Q_OBJECT

public:
    UniVision(QWidget *parent = nullptr);
    ~UniVision();

private:
};
