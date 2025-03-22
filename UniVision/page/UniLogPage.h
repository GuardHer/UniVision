#pragma once

#include <ElaDockWidget.h>
#include <ElaPlainTextEdit.h>

class UniLogPage : public ElaDockWidget
{
    Q_OBJECT
public:
    explicit UniLogPage(const QString &title, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    ~UniLogPage();

    void appendLog(const std::string &log);

private Q_SLOTS:
    void onAppendLog(const QString &log);

private:
    ElaPlainTextEdit *_logEdit{nullptr};
};
