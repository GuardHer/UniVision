#include "UniLogPage.h"

UniLogPage::UniLogPage(const QString& title, QWidget* parent, Qt::WindowFlags flags)
	: ElaDockWidget(title, parent, flags)
{
	_logEdit = new ElaPlainTextEdit(this);
	_logEdit->setReadOnly(true);
	_logEdit->setMaximumBlockCount(1000);

	this->setWidget(_logEdit);
}

UniLogPage::~UniLogPage()
{
}

void UniLogPage::appendLog(const std::string& log)
{
	QString logStr = QString::fromStdString(log);
	QMetaObject::invokeMethod(this, "onAppendLog", Qt::QueuedConnection, Q_ARG(QString, logStr));
}

void UniLogPage::onAppendLog(const QString &log)
{
	_logEdit->appendPlainText(log);
}
