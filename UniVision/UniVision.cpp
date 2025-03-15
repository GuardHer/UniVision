#include "UniVision.h"

#include <iostream>
#include <functional>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimerEvent>
#include <ElaMenuBar.h>
#include <ElaWidget.h>
#include <ElaPushButton.h>
#include <ElaToolButton.h>
#include <ElaIconButton.h>
#include <ElaToggleButton.h>
#include <ElaToggleSwitch.h>
#include <ElaToolBar.h>
#include <ElaText.h>
#include <ElaStatusBar.h>

#include "manager/AppStateManager.h"
#include "manager/UniCameraManager.h"
#include "manager/UniAlgorithmManager.h"
#include "extended/UniElaToggleButton.h"
#include "extended/UniElaText.h"
#include "page/UniMasterPage.h"
#include "config/UniSettings.h"
#include "core/UniLog.h"

UniVision::UniVision(QWidget *parent)
    : ElaWindow(parent)
{
	g_pLog->init([](const spdlog::details::log_msg& msg) {
		//std::cout << std::string(msg.payload.data(), msg.payload.size()) << std::endl;
	});

	initWindow();

    initEdgeLayout();

    initContent();

    initDevice();

    g_pUniAlgorithmManager->init();
	connect(g_pUniAlgorithmManager, &UniAlgorithmManager::resultReady, this, &UniVision::onAlgorithmResultReady);

    Q_EMIT AppStateManager::instance()->runStateChanged(AppStateManager::instance()->runState());

	startTimer(1000);
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
    connect(_runButton, &ElaToolButton::clicked, this, &UniVision::onRun);
    _toolBar->addWidget(_runButton);

    _stopButton = new ElaToolButton(this);
    _stopButton->setElaIcon(ElaIconType::CircleStop);
    _stopButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _stopButton->setText("Stop");
    connect(_stopButton, &ElaToolButton::clicked, this, &UniVision::onStop);
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

    _statusBar = new ElaStatusBar(this);
	_statusBar->setContentsMargins(10, 0, 10, 0);
    this->setStatusBar(_statusBar);
}

void UniVision::initContent()
{
    _masterPage = new UniMasterPage(this);
    addPageNode("MASTER", _masterPage, ElaIconType::House);
}

void UniVision::initDevice()
{
    // init camera
	std::vector<CameraConfig> cameraConfigs;
    UNI_SETTINGS->getCameraConfigs(cameraConfigs);

	for (const auto& config : cameraConfigs) {
		g_pUniCameraManager->addCamera(config);

		auto camera_cb = std::bind(&UniVision::onCameraGrabbing, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		g_pUniCameraManager->setGrabbingCallback(g_pUniCameraManager->generateCameraKey(config), camera_cb);

		LOG_INFO("Camera added: {} {}", config._cameraName, config._cameraIp);
    }
}

void UniVision::onCameraGrabbing(const cv::Mat& srcImg, const std::string& imageMark, uint16_t cameraIndex, uint16_t imageIndex)
{
    LOG_INFO("UniVision::onCameraGrabbing: {}, {}, {}", imageMark, cameraIndex, imageIndex);

	// process image
	AlgorithmInput input;
	input._srcImage = srcImg;
	input._cameraIndex = cameraIndex;
	input._imageIndex = imageIndex;
	input._imageMark = imageMark;
	g_pUniAlgorithmManager->process(input);
}

void UniVision::onAlgorithmResultReady(const AlgorithmInput& input, const AlgorithmOutput& output)
{
    LOG_INFO("UniVision::onAlgorithmResultReady: {}, {}, {}", input._cameraIndex, input._imageIndex, output._imageMark);

    _masterPage->showImage(output._dstImage, output._imageMark);
}

void UniVision::onRun(bool checked)
{
	AppStateManager::instance()->requestRun();

    std::vector<CameraConfig> cameraConfigs;
    UNI_SETTINGS->getCameraConfigs(cameraConfigs);

    for (const auto& config : cameraConfigs) {
        const std::string& camera_key = g_pUniCameraManager->generateCameraKey(config);

        auto camera = g_pUniCameraManager->getCameraPtr(camera_key);
        bool isOpen = camera->open();
		bool isGrabb = camera->startGrabbing();
        LOG_INFO("Camera opened: {}", camera_key);


        camera->softwareTrigger();
    }
}

void UniVision::onStop(bool checked)
{
	AppStateManager::instance()->emergencyStop();

	std::vector<CameraConfig> cameraConfigs;
	UNI_SETTINGS->getCameraConfigs(cameraConfigs);
	for (const auto& config : cameraConfigs) {
        const std::string& camera_key = g_pUniCameraManager->generateCameraKey(config);

		auto camera = g_pUniCameraManager->getCameraPtr(camera_key);
		camera->stopGrabbing();
		camera->close();
		LOG_INFO("Camera closed: {}", camera_key);
	}
}

void UniVision::timerEvent(QTimerEvent* event)
{
	LOG_TRACE("UniVision::timerEvent: {}", event->timerId());

    {
        std::unordered_map<std::string, bool> mapConnect;
        g_pUniCameraManager->getCamerasConnect(mapConnect);

		for (const auto& [cameraName, isConnected] : mapConnect) {
			if (_deviceStatus.find(cameraName) == _deviceStatus.end()) {
                UniElaText* text = new UniElaText(QString("  %1  ").arg(cameraName.c_str()), this);
                text->setTextPixelSize(14);
                text->setAutoFillBackground(true); // 允许背景填充
				_statusBar->addWidget(text);
                _deviceStatus[cameraName] = text;
			}
            UniElaText* status = _deviceStatus[cameraName];
			// set backcolor: connected: green, disconnected: red
			QColor color = isConnected ? QColor(Qt::green) : QColor(Qt::red);
			status->setBackColor(std::move(color));
		}
    }

	ElaWindow::timerEvent(event);
}
