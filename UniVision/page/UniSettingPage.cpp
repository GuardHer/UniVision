#include "UniSettingPage.h"

#include <QHBoxLayout>
#include <QFileDialog>
#include <QInputDialog>
#include <ElaToggleSwitch.h>
#include <ElaLineEdit.h>
#include <ElaText.h>
#include <ElaSuggestBox.h>
#include <ElaScrollPageArea.h>
#include <ElaIcon.h>
#include <ElaTheme.h>
#include <ElaSpinBox.h>
#include <ElaComboBox.h>
#include <ElaPushButton.h>
#include <ElaMultiSelectComboBox.h>
#include <ElaDrawerArea.h>
#include <ElaDoubleSpinBox.h>

#include "core/UniLog.h"

UniSettingPage::UniSettingPage(QWidget *parent) : ElaScrollPage(parent)
{
    setWindowTitle("Setting");

    initUI();
}

UniSettingPage::~UniSettingPage()
{
}

void UniSettingPage::initUI()
{
#pragma region 保存配置
    UniSaveConfig saveConfig = UniSettings::instance()->getSaveConfig();
    ElaText *saveText        = new ElaText(u8"保存配置", this);
    saveText->setWordWrap(false);
    saveText->setTextPixelSize(18);

    _saveConfig = new ElaPushButton(u8"保存更改", this);
    connect(_saveConfig, &ElaPushButton::clicked, this, [=]() {
        // save config
        UniSaveConfig config;
        config._isSvaeOriginalImage  = _saveOriginalImage->getIsToggled();
        config._isSaveResultImage    = _saveResultImage->getIsToggled();
        config._saveOriginalImageDir = _saveOriginalImagePath->text().toStdString();
        config._saveResultImageDir   = _saveResultImagePath->text().toStdString();
        UniSettings::instance()->setSaveConfig(config);
        LOG_INFO("Save config!");
    });

    QHBoxLayout *saveConfigLayout = new QHBoxLayout();
    saveConfigLayout->addWidget(saveText);
    saveConfigLayout->addStretch();
    saveConfigLayout->addWidget(_saveConfig);

    _saveOriginalImage = new ElaToggleSwitch(this);
    _saveResultImage   = new ElaToggleSwitch(this);
    _saveOriginalImage->setIsToggled(saveConfig._isSvaeOriginalImage);
    _saveResultImage->setIsToggled(saveConfig._isSaveResultImage);

    _saveOriginalImagePath = new ElaLineEdit(this);
    _saveOriginalImagePath->setText(saveConfig._saveOriginalImageDir.c_str());
    _saveOriginalImagePath->setFixedHeight(35);
    _saveOriginalImagePath->setFixedWidth(500);
    _saveOriginalImagePath->setClearButtonEnabled(false);
    _saveOriginalImagePath->setReadOnly(true);
    auto saveOriginalImagePathAction =
    new QAction(ElaIcon::getInstance()->getElaIcon(ElaIconType::Browser), "Search", this);
    _saveOriginalImagePath->addAction(saveOriginalImagePathAction, QLineEdit::TrailingPosition);
    connect(saveOriginalImagePathAction, &QAction::triggered, this, [=](bool checked) {
        // select dir
        QString dir = QFileDialog::getExistingDirectory(this, u8"选择文件夹", _saveOriginalImagePath->text());
        if (!dir.isEmpty()) {
            _saveOriginalImagePath->setText(dir);
            LOG_INFO("Select dir: {}", dir.toStdString());
        }
        else {
            LOG_INFO("Select dir is empty");
        }
    });

    _saveResultImagePath = new ElaLineEdit(this);
    _saveResultImagePath->setText(saveConfig._saveResultImageDir.c_str());
    _saveResultImagePath->setFixedHeight(35);
    _saveResultImagePath->setFixedWidth(500);
    _saveResultImagePath->setClearButtonEnabled(false);
    _saveResultImagePath->setReadOnly(true);
    auto saveResultImagePathAction =
    new QAction(ElaIcon::getInstance()->getElaIcon(ElaIconType::Browser), "Search", this);
    _saveResultImagePath->addAction(saveResultImagePathAction, QLineEdit::TrailingPosition);
    connect(saveResultImagePathAction, &QAction::triggered, this, [=](bool checked) {
        // select dir
        QString dir = QFileDialog::getExistingDirectory(this, u8"选择文件夹", _saveResultImagePath->text());
        if (!dir.isEmpty()) {
            _saveResultImagePath->setText(dir);
            LOG_INFO("Select dir: {}", dir.toStdString());
        }
        else {
            LOG_INFO("Select dir is empty");
        }
    });


    ElaScrollPageArea *saveOriConfigArea    = createScrollPageArea(u8"保存原图", _saveOriginalImage);
    ElaScrollPageArea *saveResultConfigArea = createScrollPageArea(u8"保存结果图", _saveResultImage);
    ElaScrollPageArea *pathOriConfigArea    = createScrollPageArea(u8"原图路径", _saveOriginalImagePath);
    ElaScrollPageArea *pathResultConfigArea = createScrollPageArea(u8"结果图路径", _saveResultImagePath);
#pragma endregion


#pragma region 算法配置
    AlgorithmConfig algorithmConfig = UniSettings::instance()->getAlgorithmConfig();
    ElaText *algorithmText          = new ElaText(u8"算法配置", this);
    algorithmText->setWordWrap(false);
    algorithmText->setTextPixelSize(18);

    _algorithmConfig = new ElaPushButton(u8"保存更改", this);
    connect(_algorithmConfig, &ElaPushButton::clicked, this, [=]() {
        // save config
        AlgorithmConfig config;
        config._algorithmName = _algorithmName->text().toStdString();
        config._detectType    = StringToAlgorithmDetectType(_algorithmModelType->currentText().toStdString());
        config._modelPath     = _algorithmModelPath->text().toStdString();
        config._modelName     = _algorithmModelName->text().toStdString();
        config._labelPath     = _algorithmLabelPath->text().toStdString();
        config._enableCuda    = _algorithmEnableCuda->getIsToggled();
        config._gpuIndex      = _algorithmGpuIndex->value();
        config._threadNum     = _algorithmThreadNum->value();
        config._param._confidenceThreshold = _algorithmParamConf->value();
        config._param._nmsThreshold        = _algorithmParamNms->value();
        config._param._meanValues          = StringToVector<float>(_algorithmParamMean->text().toStdString());
        config._param._stdValues           = StringToVector<float>(_algorithmParamStd->text().toStdString());
        config._param._minArea             = StringToVector<int>(_algorithmParamMinArea->text().toStdString());
        UniSettings::instance()->setAlgorithmConfig(config);
        LOG_INFO("Save Algorithm config!");
    });

    QHBoxLayout *algorithmConfigLayout = new QHBoxLayout();
    algorithmConfigLayout->addWidget(algorithmText);
    algorithmConfigLayout->addStretch();
    algorithmConfigLayout->addWidget(_algorithmConfig);

    _algorithmName = new ElaLineEdit(this);
    _algorithmName->setText(algorithmConfig._algorithmName.c_str());
    _algorithmName->setFixedHeight(35);
    _algorithmName->setFixedWidth(200);
    _algorithmName->setClearButtonEnabled(true);

    _algorithmModelType = new ElaComboBox(this);
    _algorithmModelType->setFixedWidth(200);
    auto vecType = AlgorithmDetectTypeToStrings();
    QStringList lstType;
    for (const auto &type : vecType) {
        lstType.push_back(type.c_str());
    }

    _algorithmModelType->addItems(lstType);
    _algorithmModelType->setCurrentText(AlgorithmDetectTypeToString(algorithmConfig._detectType).c_str());

    _algorithmModelPath = new ElaLineEdit(this);
    _algorithmModelPath->setText(algorithmConfig._modelPath.c_str());
    _algorithmModelPath->setFixedHeight(35);
    _algorithmModelPath->setFixedWidth(500);
    _algorithmModelPath->setClearButtonEnabled(false);
    _algorithmModelPath->setReadOnly(true);
    auto algorithmModelPathdAction =
    new QAction(ElaIcon::getInstance()->getElaIcon(ElaIconType::Browser), "Search", this);
    _algorithmModelPath->addAction(algorithmModelPathdAction, QLineEdit::TrailingPosition);
    connect(algorithmModelPathdAction, &QAction::triggered, this, [=](bool checked) {
        // select dir
        QString dir = QFileDialog::getExistingDirectory(this, u8"选择文件夹", _algorithmModelPath->text());
        if (!dir.isEmpty()) {
            _algorithmModelPath->setText(dir);
            LOG_INFO("Select Algo Path dir: {}", dir.toStdString());
        }
        else {
            LOG_INFO("Select Algo Path dir is empty");
        }
    });

    _algorithmModelName = new ElaLineEdit(this);
    _algorithmModelName->setText(algorithmConfig._modelName.c_str());
    _algorithmModelName->setFixedHeight(35);
    _algorithmModelName->setFixedWidth(500);
    _algorithmModelName->setClearButtonEnabled(true);
    _algorithmModelName->setReadOnly(false);
    auto algorithmModelNameAction =
    new QAction(ElaIcon::getInstance()->getElaIcon(ElaIconType::Browser), "Search", this);
    _algorithmModelName->addAction(algorithmModelNameAction, QLineEdit::TrailingPosition);
    connect(algorithmModelNameAction, &QAction::triggered, this, [=](bool checked) {
        // add file *.onnx, *.engine
        QStringList filters;
        filters << "*.onnx"
                << "*.engine";
        QString file =
        QFileDialog::getOpenFileName(this, u8"选择文件", _algorithmModelPath->text(), filters.join(";;"));
        if (!file.isEmpty()) {
            // add file
            QString fineName = QFileInfo(file).fileName();
            QString text     = _algorithmModelName->text();
            if (text.isEmpty()) {
                _algorithmModelName->setText(fineName);
            }
            else {
                _algorithmModelName->setText(text + "," + fineName);
            }

            LOG_INFO("Add model: {}", fineName.toStdString());
        }
        else {
            LOG_INFO("Select model is empty");
        }
    });

    _algorithmLabelPath = new ElaLineEdit(this);
    _algorithmLabelPath->setText(algorithmConfig._labelPath.c_str());
    _algorithmLabelPath->setFixedHeight(35);
    _algorithmLabelPath->setFixedWidth(500);
    _algorithmLabelPath->setClearButtonEnabled(false);
    _algorithmLabelPath->setReadOnly(true);
    auto algorithmLabelPathAction =
    new QAction(ElaIcon::getInstance()->getElaIcon(ElaIconType::Browser), "Search", this);
    _algorithmLabelPath->addAction(algorithmLabelPathAction, QLineEdit::TrailingPosition);
    connect(algorithmLabelPathAction, &QAction::triggered, this, [=](bool checked) {
        // select file
        QString file = QFileDialog::getOpenFileName(this, u8"选择文件", _algorithmLabelPath->text());
        if (!file.isEmpty()) {
            _algorithmLabelPath->setText(file);
            LOG_INFO("Select label: {}", file.toStdString());
        }
        else {
            LOG_INFO("Select label is empty");
        }
    });

    _algorithmEnableCuda = new ElaToggleSwitch(this);
    _algorithmEnableCuda->setIsToggled(algorithmConfig._enableCuda);

    _algorithmGpuIndex = new ElaSpinBox(this);
    _algorithmGpuIndex->setRange(0, 100);
    _algorithmGpuIndex->setValue((int) algorithmConfig._gpuIndex);

    _algorithmThreadNum = new ElaSpinBox(this);
    _algorithmThreadNum->setRange(0, 100);
    _algorithmThreadNum->setValue((int) algorithmConfig._threadNum);

    // 算法参数
    ElaDrawerArea *algorithmParamDrawerArea = new ElaDrawerArea(this);
    _algorithmParamConf                     = new ElaDoubleSpinBox(this);
    _algorithmParamConf->setRange(0, 1);
    _algorithmParamConf->setValue(algorithmConfig._param._confidenceThreshold);

    _algorithmParamNms = new ElaDoubleSpinBox(this);
    _algorithmParamNms->setRange(0, 1);
    _algorithmParamNms->setValue(algorithmConfig._param._nmsThreshold);

    _algorithmParamMean = new ElaLineEdit(this);
    _algorithmParamMean->setText(QString::fromStdString(VectorToString(algorithmConfig._param._meanValues)));
    _algorithmParamMean->setFixedHeight(35);
    _algorithmParamMean->setFixedWidth(500);
    _algorithmParamMean->setClearButtonEnabled(true);
    _algorithmParamMean->setReadOnly(false);
    auto algorithmParamMeanAction =
    new QAction(ElaIcon::getInstance()->getElaIcon(ElaIconType::Browser), "Search", this);
    _algorithmParamMean->addAction(algorithmParamMeanAction, QLineEdit::TrailingPosition);
    connect(algorithmParamMeanAction, &QAction::triggered, this, [=](bool checked) {
        // get float
        float value = QInputDialog::getDouble(this, u8"均值", u8"输入均值", 0.0, -100000, 100000, 2);
        if (value != 0.0) {
            QString text = _algorithmParamMean->text();
            if (text.isEmpty()) {
                _algorithmParamMean->setText(QString::number(value));
            }
            else {
                _algorithmParamMean->setText(text + "," + QString::number(value, 'f', 2));
            }
            LOG_INFO("Add mean: {}", value);
        }
        else {
            LOG_INFO("Input mean is empty");
        }
    });

    _algorithmParamStd = new ElaLineEdit(this);
    _algorithmParamStd->setText(QString::fromStdString(VectorToString(algorithmConfig._param._stdValues)));
    _algorithmParamStd->setFixedHeight(35);
    _algorithmParamStd->setFixedWidth(500);
    _algorithmParamStd->setClearButtonEnabled(true);
    _algorithmParamStd->setReadOnly(false);
    auto algorithmParamStdAction =
    new QAction(ElaIcon::getInstance()->getElaIcon(ElaIconType::Browser), "Search", this);
    _algorithmParamStd->addAction(algorithmParamStdAction, QLineEdit::TrailingPosition);
    connect(algorithmParamStdAction, &QAction::triggered, this, [=](bool checked) {
        // get float
        float value = QInputDialog::getDouble(this, u8"标准差", u8"输入标准差", 0.0, -100000, 100000, 2);
        if (value != 0.0) {
            QString text = _algorithmParamStd->text();
            if (text.isEmpty()) {
                _algorithmParamStd->setText(QString::number(value));
            }
            else {
                _algorithmParamStd->setText(text + "," + QString::number(value, 'f', 2));
            }
            LOG_INFO("Add std: {}", value);
        }
        else {
            LOG_INFO("Input std is empty");
        }
    });

    _algorithmParamMinArea = new ElaLineEdit(this);
    _algorithmParamMinArea->setText(QString::fromStdString(VectorToString(algorithmConfig._param._stdValues)));
    _algorithmParamMinArea->setFixedHeight(35);
    _algorithmParamMinArea->setFixedWidth(500);
    _algorithmParamMinArea->setClearButtonEnabled(true);
    _algorithmParamMinArea->setReadOnly(false);
    auto algorithmParamAreaAction =
    new QAction(ElaIcon::getInstance()->getElaIcon(ElaIconType::Browser), "Search", this);
    _algorithmParamMinArea->addAction(algorithmParamAreaAction, QLineEdit::TrailingPosition);
    connect(algorithmParamAreaAction, &QAction::triggered, this, [=](bool checked) {
        // get float
        int value = QInputDialog::getInt(this, u8"最小面积", u8"输入面积", 0, 0, 10000000, 100);
        if (value != 0) {
            QString text = _algorithmParamMinArea->text();
            if (text.isEmpty()) {
                _algorithmParamMinArea->setText(QString::number(value));
            }
            else {
                _algorithmParamMinArea->setText(text + "," + QString::number(value));
            }
            LOG_INFO("Add area: {}", value);
        }
        else {
            LOG_INFO("Input area is empty");
        }
    });

    {
        auto *algorithmParamConfArea = createSettingWidget(u8"置信度", _algorithmParamConf);
        auto *algorithmParamNmsArea  = createSettingWidget(u8"NMS", _algorithmParamNms);
        auto *algorithmParamMeanArea = createSettingWidget(u8"均值", _algorithmParamMean);
        auto *algorithmParamStdArea  = createSettingWidget(u8"标准差", _algorithmParamStd);
        auto *algorithmParamAreaArea = createSettingWidget(u8"最小面积", _algorithmParamMinArea);

        QWidget *drawerHeader           = new QWidget(this);
        QHBoxLayout *drawerHeaderLayout = new QHBoxLayout(drawerHeader);
        ElaText *drawerIcon             = new ElaText(this);
        drawerIcon->setTextPixelSize(15);
        drawerIcon->setElaIcon(ElaIconType::MessageArrowDown);
        drawerIcon->setFixedSize(25, 25);
        ElaText *drawerText = new ElaText(u8"算法参数", this);
        drawerText->setTextPixelSize(15);

        drawerHeaderLayout->addWidget(drawerIcon);
        drawerHeaderLayout->addWidget(drawerText);
        drawerHeaderLayout->addStretch();

        algorithmParamDrawerArea->setDrawerHeader(drawerHeader);

        algorithmParamDrawerArea->addDrawer(algorithmParamConfArea);
        algorithmParamDrawerArea->addDrawer(algorithmParamNmsArea);
        algorithmParamDrawerArea->addDrawer(algorithmParamMeanArea);
        algorithmParamDrawerArea->addDrawer(algorithmParamStdArea);
        algorithmParamDrawerArea->addDrawer(algorithmParamAreaArea);
    }


    ElaScrollPageArea *algorithmNameConfigArea       = createScrollPageArea(u8"算法名称", _algorithmName);
    ElaScrollPageArea *algorithmModelTypeConfigArea  = createScrollPageArea(u8"算法类型", _algorithmModelType);
    ElaScrollPageArea *algorithmModelPathConfigArea  = createScrollPageArea(u8"模型路径", _algorithmModelPath);
    ElaScrollPageArea *algorithmModelNameConfigArea  = createScrollPageArea(u8"模型名称", _algorithmModelName);
    ElaScrollPageArea *algorithmLabelPathConfigArea  = createScrollPageArea(u8"标签路径", _algorithmLabelPath);
    ElaScrollPageArea *algorithmEnableCudaConfigArea = createScrollPageArea(u8"启用CUDA", _algorithmEnableCuda);
    ElaScrollPageArea *algorithmGpuIndexConfigArea   = createScrollPageArea(u8"GPU索引", _algorithmGpuIndex);
    ElaScrollPageArea *algorithmThreadNumConfigArea  = createScrollPageArea(u8"线程数量", _algorithmThreadNum);


#pragma endregion


#pragma region END

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("Setting");
    QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addSpacing(30);
    centerLayout->addLayout(saveConfigLayout);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(saveOriConfigArea);
    centerLayout->addWidget(saveResultConfigArea);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(pathOriConfigArea);
    centerLayout->addWidget(pathResultConfigArea);

    centerLayout->addSpacing(30);
    centerLayout->addLayout(algorithmConfigLayout);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(algorithmNameConfigArea);
    centerLayout->addWidget(algorithmModelTypeConfigArea);
    centerLayout->addWidget(algorithmModelPathConfigArea);
    centerLayout->addWidget(algorithmLabelPathConfigArea);
    centerLayout->addWidget(algorithmModelNameConfigArea);
    centerLayout->addWidget(algorithmEnableCudaConfigArea);
    centerLayout->addWidget(algorithmGpuIndexConfigArea);
    centerLayout->addWidget(algorithmThreadNumConfigArea);
    centerLayout->addWidget(algorithmParamDrawerArea);

    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
#pragma endregion
}

QWidget *UniSettingPage::createSettingWidget(const QString &title, QWidget *widget, QWidget *widget2)
{
    QWidget *saveConfigArea       = new QWidget(this);
    QHBoxLayout *saveConfigLayout = new QHBoxLayout(saveConfigArea);
    ElaText *saveOriImageText     = new ElaText(title, this);
    saveConfigArea->setMinimumHeight(widget->height());
    saveOriImageText->setWordWrap(false);
    saveOriImageText->setTextPixelSize(15);
    saveConfigLayout->addWidget(saveOriImageText);
    if (widget2) {
        saveConfigLayout->addSpacing(20);
        saveConfigLayout->addWidget(widget2);
    }
    saveConfigLayout->setSpacing(0);
    //saveConfigLayout->setContentsMargins(20, 0, 20, 0);
    saveConfigLayout->addStretch();
    saveConfigLayout->addWidget(widget);

    return saveConfigArea;
}

ElaScrollPageArea *UniSettingPage::createScrollPageArea(const QString &title, QWidget *widget, QWidget *widget2)
{
    ElaScrollPageArea *saveConfigArea = new ElaScrollPageArea(this);
    QHBoxLayout *saveConfigLayout     = new QHBoxLayout(saveConfigArea);
    ElaText *saveOriImageText         = new ElaText(title, this);
    saveOriImageText->setWordWrap(false);
    saveOriImageText->setTextPixelSize(15);
    saveConfigLayout->addWidget(saveOriImageText);
    if (widget2) {
        saveConfigLayout->addSpacing(20);
        saveConfigLayout->addWidget(widget2);
    }
    saveConfigLayout->setSpacing(0);
    saveConfigLayout->setContentsMargins(20, 0, 20, 0);
    saveConfigLayout->addStretch();
    saveConfigLayout->addWidget(widget);

    return saveConfigArea;
}
