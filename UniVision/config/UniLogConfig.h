#pragma once

#include <string>

struct UniLogConfig
{
	std::string _logName;     // 日志名称
	std::string _logSavePath; // 日志保存路径
	std::string _logPattern;  // 日志格式
	int _logRotateHour;       // 日志轮转时间
	int _logRotateMinute;     // 日志轮转分钟
	int _level;               // 日志级别
};

