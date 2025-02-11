/********************************************************************************
** Form generated from reading UI file 'UniVision.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UNIVISION_H
#define UI_UNIVISION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UniVisionClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *UniVisionClass)
    {
        if (UniVisionClass->objectName().isEmpty())
            UniVisionClass->setObjectName(QString::fromUtf8("UniVisionClass"));
        UniVisionClass->resize(600, 400);
        menuBar = new QMenuBar(UniVisionClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        UniVisionClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(UniVisionClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        UniVisionClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(UniVisionClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        UniVisionClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(UniVisionClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        UniVisionClass->setStatusBar(statusBar);

        retranslateUi(UniVisionClass);

        QMetaObject::connectSlotsByName(UniVisionClass);
    } // setupUi

    void retranslateUi(QMainWindow *UniVisionClass)
    {
        UniVisionClass->setWindowTitle(QCoreApplication::translate("UniVisionClass", "UniVision", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UniVisionClass: public Ui_UniVisionClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UNIVISION_H
