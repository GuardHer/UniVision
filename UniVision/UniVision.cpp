#include "UniVision.h"

#include <iostream>
#include <functional>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimerEvent>
#include <QTimer>

#include <ElaMenuBar.h>
#include <ElaWidget.h>
#include <ElaPushButton.h>
#include <ElaToolButton.h>
#include <ElaIconButton.h>
#include <ElaToggleButton.h>
#include <ElaToggleSwitch.h>
#include <ElaToolBar.h>
#include <ElaText.h>
#include <ElaSpinBox.h>
#include <ElaStatusBar.h>
#include <ElaMultiSelectComboBox.h>
#include <ElaMessageBar.h>

#include "manager/AppStateManager.h"
#include "manager/UniCameraManager.h"
#include "manager/UniAlgorithmManager.h"
#include "algorithm/UniAlgorithm.h"
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

    initAlgorithm();

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
    _statusButton->setFixedSize(100, 32);
    _toolBar->addWidget(_statusButton);

    // 模拟触发相机
    _toolBar->addSeparator();
	_selectCameras = new ElaMultiSelectComboBox(this);
    std::vector<CameraConfig> cameraConfigs;
    UNI_SETTINGS->getCameraConfigs(cameraConfigs);
	for (const auto& config : cameraConfigs) {
		_selectCameras->addItem(QString::fromStdString(config._cameraName));
	}
	_selectCameras->setFixedSize(120, 32);
	_toolBar->addWidget(_selectCameras);

    _singleTrigger = new ElaToolButton(this);
    _singleTrigger->setElaIcon(ElaIconType::ArrowRightToArc);
    _singleTrigger->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _singleTrigger->setText("Single Trigger");
    _toolBar->addWidget(_singleTrigger);
	connect(_singleTrigger, &ElaToolButton::clicked, this, &UniVision::onSimSingleTrigger);

    _toolBar->addSeparator();

	_timeInterval = new ElaSpinBox(this);
	_timeInterval->setRange(100, 100000);
	_timeInterval->setValue(1000);
	_timeInterval->setSingleStep(100);
	_timeInterval->setSuffix(" ms");
	_toolBar->addWidget(_timeInterval);

    _triggerCount = new ElaSpinBox(this);
    _triggerCount->setRange(0, 100000);
    _triggerCount->setValue(5);
    _triggerCount->setSingleStep(1);
    _triggerCount->setSuffix(" 次 "); 
    _toolBar->addWidget(_triggerCount);

    _timerTrigger = new ElaToolButton(this);
    _timerTrigger->setElaIcon(ElaIconType::Timer);
    _timerTrigger->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _timerTrigger->setText("Timer Trigger");
	_toolBar->addWidget(_timerTrigger);
	connect(_timerTrigger, &ElaToolButton::clicked, this, &UniVision::onSimTimerTrigger);

	_timerStatus = new UniElaToggleButton(this);
    _timerStatus->setFixedSize(100, 32);
    _timerStatus->setIsToggled(false);
    _timerStatus->setText(u8"未触发...");
    _toolBar->addWidget(_timerStatus);

	_simTriggerTimer = new QTimer(this);
	connect(_simTriggerTimer, &QTimer::timeout, this, &UniVision::onSimTriggerTimeout);

    _toolBar->addSeparator();


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

void UniVision::initAlgorithm()
{
    g_pUniAlgorithmManager->init();

    auto cb = std::bind(&UniVision::onAlgorithmResultReady, this, std::placeholders::_1, std::placeholders::_2);
    g_pUniAlgorithm->setAlgorithmResultReadyCallback(std::move(cb));
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

    _masterPage->showImage(output);
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

	_simTriggerTimer->stop();
}

void UniVision::onSimTimerTrigger(bool checked)
{

	if (_simTriggerTimer->isActive()) {
		_simTriggerTimer->stop();
		_timerStatus->setIsToggled(false);
		_timerStatus->setText(u8"未触发...");
	}
	else {
		auto status = AppStateManager::instance()->runState();
		if (status != AppStateManager::RunState::Running) {
			ElaMessageBar::warning(ElaMessageBarType::Top, "warning", u8"请先开始运行再执行软触发!", 2, this);
			return;
		}

		_simTriggerTimer->start(_timeInterval->value());
		_timerStatus->setIsToggled(true);
		_timerStatus->setText(u8"已触发...");
	}

}

void UniVision::onSimSingleTrigger(bool checked)
{
	auto status = AppStateManager::instance()->runState();
	if (status != AppStateManager::RunState::Running) {
		ElaMessageBar::warning(ElaMessageBarType::Top, "warning", u8"请先开始运行再执行软触发!", 2, this);
		return;
	}

	std::vector<CameraConfig> cameraConfigs;
	UNI_SETTINGS->getCameraConfigs(cameraConfigs);

    QStringList selectedCameras = _selectCameras->getCurrentSelection();

	for (const auto& config : cameraConfigs) {
		if (!selectedCameras.contains(QString::fromStdString(config._cameraName))) {
			continue;
		}

		const std::string& camera_key = g_pUniCameraManager->generateCameraKey(config);

		auto camera = g_pUniCameraManager->getCameraPtr(camera_key);
		camera->softwareTrigger();
	}
}

void UniVision::onSimTriggerTimeout()
{
	int count = _triggerCount->value();
	if (count > 0) {
		_triggerCount->setValue(count - 1);
	}
	else {
		_simTriggerTimer->stop();
		_timerStatus->setIsToggled(false);
		_timerStatus->setText(u8"未触发...");

		return;
	}

	std::vector<CameraConfig> cameraConfigs;
	UNI_SETTINGS->getCameraConfigs(cameraConfigs);

	QStringList selectedCameras = _selectCameras->getCurrentSelection();

	for (const auto& config : cameraConfigs) {
		if (!selectedCameras.contains(QString::fromStdString(config._cameraName))) {
			continue;
		}

		const std::string& camera_key = g_pUniCameraManager->generateCameraKey(config);

		auto camera = g_pUniCameraManager->getCameraPtr(camera_key);
		camera->softwareTrigger();
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
