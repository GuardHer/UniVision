#pragma once

#include <QtWidgets/QMainWindow>
#include <ElaWindow.h>

class ElaToolBar;
class ElaToolButton;
class ElaToggleButton;
class UniElaToggleButton;

class UniMasterPage;

class UniVision : public ElaWindow
{
    Q_OBJECT

public:
    UniVision(QWidget *parent = nullptr);
    ~UniVision();

private:

    void initWindow();
    void initEdgeLayout();
    void initContent();

private:
	ElaToolBar* _toolBar{ nullptr };
	ElaToolButton* _runButton{ nullptr };
	ElaToolButton* _stopButton{ nullptr };
    UniElaToggleButton* _statusButton{ nullptr };
    UniMasterPage* _masterPage{ nullptr };
};
