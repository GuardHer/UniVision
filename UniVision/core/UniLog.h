#pragma once

#include <mutex>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/callback_sink.h>

#define g_pLog UniLog::instance()
class UniLog
{
public:
	using LogCallback = std::function<void(const std::string& msg)>;

public:
	static UniLog* instance();
	void init();

	void setLogCallback(const LogCallback& callback) { _callback = callback; }

private:
	UniLog();
	~UniLog();

	void Callback(const spdlog::details::log_msg& msg);

private:
	static UniLog* _instance;
	static std::mutex _mutex;

	LogCallback _callback;
};


#define LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__)


