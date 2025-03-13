#include "AppStateManager.h"

AppStateManager* AppStateManager::instance()
{
    static AppStateManager instance;
    return &instance;
}

AppStateManager::RunState AppStateManager::runState() const
{
    QMutexLocker locker(&m_mutex);
    return m_runState;
}

AppStateManager::AlarmLevel AppStateManager::alarmLevel() const
{
    QMutexLocker locker(&m_mutex);
    return m_alarmLevel;
}

QList<AppStateManager::AlarmInfo> AppStateManager::activeAlarms() const
{
    QMutexLocker locker(&m_mutex);
    return m_activeAlarms;
}

AppStateManager::AppStateManager(QObject* parent)
    : QObject(parent)
{
}

bool AppStateManager::requestRun()
{
    QMutexLocker locker(&m_mutex);

    if (!checkPreconditions(Running)) {
        emit operationDenied("Preconditions not met");
        return false;
    }

    if (m_runState == Running) return true;

    RunState oldState = m_runState;
    m_runState = Running;
    locker.unlock();

    emit runStateChanged(m_runState);
    if (oldState == Paused) {
        // 处理从暂停恢复的特殊逻辑
    }
    return true;
}

void AppStateManager::emergencyStop()
{
    QMutexLocker locker(&m_mutex);
    if (m_runState == Stopped) return;

    m_runState = Stopped;
    locker.unlock();

    emit runStateChanged(m_runState);
    triggerAlarm(0xE001, "Emergency Stop Activated", Critical);
}

bool AppStateManager::pauseOperation()
{
    return true;
}

bool AppStateManager::resumeOperation()
{
    return true;
}

bool AppStateManager::checkPreconditions(RunState targetState)
{
    // 示例检查项：
    if (m_alarmLevel >= Critical) return false;
    if (targetState == Running && m_runState == Paused) {
        return !m_activeAlarms.isEmpty();
    }
    return true;
}

void AppStateManager::triggerAlarm(int code, const QString& desc, AlarmLevel level)
{
    QMutexLocker locker(&m_mutex);

    AlarmInfo alarm;
    alarm.timestamp = QDateTime::currentDateTime();
    alarm.code = code;
    alarm.description = desc;
    alarm.level = level;

    m_activeAlarms.append(alarm);
    locker.unlock();

    updateAlarmStatus();

    emit alarmTriggered(alarm);
}

void AppStateManager::acknowledgeAlarm(int code)
{
    QMutexLocker locker(&m_mutex);

    for (auto& alarm : m_activeAlarms) {
        if (alarm.code == code && !alarm.acknowledged) {
            alarm.acknowledged = true;
            locker.unlock();

            emit alarmAcknowledged(code);
            updateAlarmStatus();
            return;
        }
    }
}

void AppStateManager::updateAlarmStatus()
{
    QMutexLocker locker(&m_mutex);

    AlarmLevel maxLevel = NoAlarm;
    AlarmLevel currLevel = m_alarmLevel;
    for (const auto& alarm : m_activeAlarms) {
        if (!alarm.acknowledged && alarm.level > maxLevel) {
            maxLevel = alarm.level;
        }
    }
	locker.unlock();

    if (currLevel != maxLevel) {
        currLevel = maxLevel;
        emit alarmLevelChanged(currLevel);

        if (maxLevel >= Critical) {
            emergencyStop(); // 严重报警触发急停
        }
    }
}