#include "UniLog.h"
#include <memory>
#include <iostream>

#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/details/thread_pool.h>

#include "config/UniSettings.h"

UniLog* UniLog::_instance = nullptr;
std::mutex UniLog::_mutex;

UniLog* UniLog::instance()
{
	if (_instance == nullptr)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_instance == nullptr)
		{
			_instance = new UniLog();
		}
	}
	return _instance;
}
UniLog::UniLog()
{
}

UniLog::~UniLog()
{
}

void UniLog::init(spdlog::custom_log_callback cb)
{
    try
    {
        auto config = UNI_SETTINGS->getLogConfig();

        std::vector<spdlog::sink_ptr> vecSinks;

        auto cb_sink = std::make_shared<spdlog::sinks::callback_sink_mt>(std::move(cb));
        cb_sink->set_level(static_cast<spdlog::level::level_enum>(config._level));
        vecSinks.push_back(cb_sink);

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(static_cast<spdlog::level::level_enum>(config._level));
        vecSinks.push_back(console_sink);

        auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(config._logSavePath, config._logRotateHour, config._logRotateMinute);
        file_sink->set_level(static_cast<spdlog::level::level_enum>(config._level));
        vecSinks.push_back(file_sink);
        
        spdlog::init_thread_pool(10240, 2);
        auto tp = spdlog::thread_pool();
        auto logger = std::make_shared<spdlog::async_logger>(config._logName, vecSinks.begin(), vecSinks.end(), tp, spdlog::async_overflow_policy::block);
        logger->set_level(static_cast<spdlog::level::level_enum>(config._level));
        if (!config._logPattern.empty())
        {
            logger->set_pattern(config._logPattern);
        }

        spdlog::flush_every(std::chrono::seconds(3));
        spdlog::register_logger(logger);
        spdlog::set_default_logger(logger);
    }
    catch (spdlog::spdlog_ex e)
    {
        std::cout << e.what();
    }
}

