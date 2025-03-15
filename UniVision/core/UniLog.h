#pragma once

#include <mutex>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/callback_sink.h>

#define g_pLog UniLog::instance()
class UniLog
{
public:
	static UniLog* instance();

	void init(spdlog::custom_log_callback cb);

private:
	UniLog();
	~UniLog();

private:
	static UniLog* _instance;
	static std::mutex _mutex;
};

#define LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__)


