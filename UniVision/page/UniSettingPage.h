#pragma once

#include <ElaScrollPage.h>
#include "config/UniSettings.h"

class ElaSpinBox;
class ElaComboBox;
class ElaLineEdit;
class ElaPushButton;
class ElaToggleSwitch;
class ElaDoubleSpinBox;
class ElaScrollPageArea;
class ElaMultiSelectComboBox;

class UniSettingPage : public ElaScrollPage
{
    Q_OBJECT
public:
    explicit UniSettingPage(QWidget *parent = nullptr);
    ~UniSettingPage();

    void setSettingKey(const QString &key) { _settingKey = key; };

private:
    void initUI();

private:
    QWidget *createSettingWidget(const QString &title, QWidget *widget, QWidget *widget2 = nullptr);
    ElaScrollPageArea *createScrollPageArea(const QString &title, QWidget *widget, QWidget *widget2 = nullptr);

private:
    QString _settingKey;

    // 保存配置
    ElaPushButton *_saveConfig{nullptr};          // 保存配置按钮
    ElaToggleSwitch *_saveOriginalImage{nullptr}; // 保存原图
    ElaToggleSwitch *_saveResultImage{nullptr};   // 保存结果图
    ElaLineEdit *_saveOriginalImagePath{nullptr}; // 保存原图路径
    ElaLineEdit *_saveResultImagePath{nullptr};   // 保存结果图路径

    // 算法配置
    ElaPushButton *_algorithmConfig{nullptr};       // 算法配置按钮
    ElaLineEdit *_algorithmName{nullptr};           // 算法名称
    ElaLineEdit *_algorithmModelPath{nullptr};      // 算法模型路径
    ElaLineEdit *_algorithmModelName{nullptr};      // 算法模型名称
    ElaComboBox *_algorithmModelType{nullptr};      // 算法模型类型
    ElaLineEdit *_algorithmLabelPath{nullptr};      // 算法标签路径
    ElaToggleSwitch *_algorithmEnableCuda{nullptr}; // 算法是否启用cuda
    ElaSpinBox *_algorithmGpuIndex{nullptr};        // 算法gpu索引
    ElaSpinBox *_algorithmThreadNum{nullptr};       // 算法线程数

    // 算法参数
    ElaDoubleSpinBox *_algorithmParamConf{nullptr}; // 算法参数置信度
    ElaDoubleSpinBox *_algorithmParamNms{nullptr};  // 算法参数NMS
    ElaLineEdit *_algorithmParamMean{nullptr};      // 算法参数均值
    ElaLineEdit *_algorithmParamStd{nullptr};       // 算法参数标准差
    ElaLineEdit *_algorithmParamMinArea{nullptr};   // 算法参数最小面积

};
