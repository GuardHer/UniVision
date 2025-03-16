#pragma once

#include <QtWidgets/QMainWindow>
#include <ElaWindow.h>

#include <opencv2/opencv.hpp>
#include <string>

#include "config/UniAlgorithmConfig.h"

class QTimer;
class ElaText;
class UniElaText;
class ElaSpinBox;
class ElaToolBar;
class ElaStatusBar;
class ElaToolButton;
class ElaToggleButton;
class UniElaToggleButton;
class ElaMultiSelectComboBox;

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

	void initDevice();
	void initAlgorithm();

	void onCameraGrabbing(const cv::Mat& srcImg,
		const std::string& imageMark,
		uint16_t cameraIndex,
		uint16_t imageIndex);

private Q_SLOTS:
	void onRun(bool checked);
	void onStop(bool checked);

	void onSimTimerTrigger(bool checked);
	void onSimSingleTrigger(bool checked);
	void onSimTriggerTimeout();


private:
	void onAlgorithmResultReady(const AlgorithmInput& input, const AlgorithmOutput& output);


protected:
	// timeEvent
	void timerEvent(QTimerEvent* event) override;

private:
	ElaToolBar* _toolBar{ nullptr };
	ElaToolButton* _runButton{ nullptr };
	ElaToolButton* _stopButton{ nullptr };
    UniElaToggleButton* _statusButton{ nullptr };
    UniMasterPage* _masterPage{ nullptr };
	ElaStatusBar* _statusBar{ nullptr };

	// 模拟触发
	ElaMultiSelectComboBox* _selectCameras{ nullptr };
	ElaSpinBox* _timeInterval{ nullptr };  // 时间间隔 ms
	ElaSpinBox* _triggerCount{ nullptr };  // 触发次数
	UniElaToggleButton* _timerStatus{ nullptr }; // 定时状态
	QTimer* _simTriggerTimer{ nullptr }; // 模拟触发定时器
	ElaToolButton* _timerTrigger{ nullptr };  // 定时触发
	ElaToolButton* _singleTrigger{ nullptr }; // 单次触发

	std::unordered_map<std::string, UniElaText*> _deviceStatus;
};
