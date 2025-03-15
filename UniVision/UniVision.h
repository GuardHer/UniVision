#pragma once

#include <QtWidgets/QMainWindow>
#include <ElaWindow.h>

#include <opencv2/opencv.hpp>
#include <string>

#include "config/UniAlgorithmConfig.h"

class UniElaText;
class ElaText;
class ElaToolBar;
class ElaStatusBar;
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

	void initDevice();

	void onCameraGrabbing(const cv::Mat& srcImg,
		const std::string& imageMark,
		uint16_t cameraIndex,
		uint16_t imageIndex);

private Q_SLOTS:
	void onRun(bool checked);
	void onStop(bool checked);

	// UniAlgorithmManager::resultReady
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

	std::unordered_map<std::string, UniElaText*> _deviceStatus;
};
