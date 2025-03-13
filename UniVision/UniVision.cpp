#include "UniVision.h"

#include <QVBoxLayout>

#include <ElaMenuBar.h>
#include <ElaWidget.h>
#include <ElaPushButton.h>
#include <ElaToolButton.h>
#include <ElaIconButton.h>
#include <ElaToggleButton.h>
#include <ElaToggleSwitch.h>
#include <ElaToolBar.h>
#include <ElaText.h>

#include "manager/AppStateManager.h"
#include "extended/UniElaToggleButton.h"
#include "page/UniMasterPage.h"
#include "config/UniSettings.h"

UniVision::UniVision(QWidget *parent)
    : ElaWindow(parent)
{
	initWindow();

    initEdgeLayout();

    initContent();

    Q_EMIT AppStateManager::instance()->runStateChanged(AppStateManager::instance()->runState());
}

UniVision::~UniVision()
{
}

void UniVision::initWindow()
{
	this->setWindowIcon(QIcon(":/include/Image/Cirno.jpg"));
	this->setWindowTitle("UniVision");
	this->setNavigationBarDisplayMode(ElaNavigationType::Compact);
}

void UniVision::initEdgeLayout()
{
	_toolBar = new ElaToolBar("工具栏", this);
    _toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    _toolBar->setToolBarSpacing(3);
    _toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    //_toolBar->setFloatable(false);
    //_toolBar->setMovable(false);
    _toolBar->setIconSize(QSize(32, 32));

	_runButton = new ElaToolButton(this);
    _runButton->setElaIcon(ElaIconType::CaretRight);
    _runButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _runButton->setText("Run");
    connect(_runButton, &ElaToolButton::clicked, this, [this](bool checked) {
        AppStateManager::instance()->requestRun();
        });
    _toolBar->addWidget(_runButton);

    _stopButton = new ElaToolButton(this);
    _stopButton->setElaIcon(ElaIconType::CircleStop);
    _stopButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _stopButton->setText("Stop");
    connect(_stopButton, &ElaToolButton::clicked, this, [this](bool checked) {
		AppStateManager::instance()->emergencyStop();
        });
    _toolBar->addWidget(_stopButton);

    _toolBar->addSeparator();

    _statusButton = new UniElaToggleButton(this);
    connect(AppStateManager::instance(), &AppStateManager::runStateChanged, this, [this](AppStateManager::RunState newState) {
		if (newState == AppStateManager::Running) {
			_statusButton->setIsToggled(true);
			_statusButton->setText(u8"运行中...");

		}
        else {
            _statusButton->setIsToggled(false);
            _statusButton->setText(u8"停止中...");
        }
    });
    _statusButton->setFixedWidth(100);
    _toolBar->addWidget(_statusButton);

    this->addToolBar(Qt::TopToolBarArea, _toolBar);
}

void UniVision::initContent()
{
    _masterPage = new UniMasterPage(this);
    addPageNode("MASTER", _masterPage, ElaIconType::House);
}
