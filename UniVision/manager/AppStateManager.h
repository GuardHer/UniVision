#pragma once

#include <QObject>
#include <QMutex>
#include <QDateTime>

class AppStateManager : public QObject
{
    Q_OBJECT
        Q_PROPERTY(RunState runState READ runState NOTIFY runStateChanged)
        Q_PROPERTY(AlarmLevel alarmLevel READ alarmLevel NOTIFY alarmLevelChanged)

public:
    enum RunState {
        Stopped = 0,
        Running = 1,
        Paused = 2
    };
    Q_ENUM(RunState)

    enum AlarmLevel {
        NoAlarm = 0,
        Warning = 1,
        Critical = 2
    };
    Q_ENUM(AlarmLevel)

        struct AlarmInfo {
        QDateTime timestamp;
        AlarmLevel level;
        int code;
        QString description;
        bool acknowledged = false;
    };

    static AppStateManager* instance();

    // 状态获取
    RunState runState() const;
    AlarmLevel alarmLevel() const;
    QList<AlarmInfo> activeAlarms() const;

    // 状态控制
    Q_INVOKABLE bool requestRun();
    Q_INVOKABLE void emergencyStop();
    Q_INVOKABLE bool pauseOperation();
    Q_INVOKABLE bool resumeOperation();

    // 报警处理
    Q_INVOKABLE void acknowledgeAlarm(int code);
    void triggerAlarm(int code, const QString& description, AlarmLevel level);

Q_SIGNALS:
    Q_SIGNAL void runStateChanged(RunState newState);
    Q_SIGNAL void alarmLevelChanged(AlarmLevel newLevel);
    Q_SIGNAL void alarmTriggered(const AlarmInfo& alarm);
    Q_SIGNAL void alarmAcknowledged(int code);
    Q_SIGNAL void operationDenied(const QString& reason);

protected:
    explicit AppStateManager(QObject* parent = nullptr);

private:
    bool checkPreconditions(RunState targetState);
    void updateAlarmStatus();

private:

    mutable QMutex m_mutex;
    RunState m_runState = Stopped;
    AlarmLevel m_alarmLevel = NoAlarm;
    QList<AlarmInfo> m_activeAlarms;
};

