#pragma once

#include <string>
#include <spdlog/spdlog.h>

namespace assfire {
    namespace log {
        void initializeSpdlog(std::string log_level)
        {
            if (log_level == "critical") {
                SPDLOG_INFO("Setting log level to CRITICAL");
                spdlog::set_level(spdlog::level::critical);
            }
            else if (log_level == "err") {
                SPDLOG_INFO("Setting log level to ERROR");
                spdlog::set_level(spdlog::level::err);
            }
            else if (log_level == "warn") {
                SPDLOG_INFO("Setting log level to WARN");
                spdlog::set_level(spdlog::level::warn);
            }
            else if (log_level == "info") {
                SPDLOG_INFO("Setting log level to INFO");
                spdlog::set_level(spdlog::level::info);
            }
            else if (log_level == "debug") {
                SPDLOG_INFO("Setting log level to DEBUG");
                spdlog::set_level(spdlog::level::debug);
            }
            else if (log_level == "trace") {
                SPDLOG_INFO("Setting log level to TRACE");
                spdlog::set_level(spdlog::level::trace);
            } else {
                SPDLOG_ERROR("Unknown log level selected: {}", log_level);
            }

#ifdef ASSFIRE_DEBUG_MODE
            SPDLOG_CRITICAL("This is a sample critical message");
            SPDLOG_ERROR("This is a sample err message");
            SPDLOG_WARN("This is a sample warn message");
            SPDLOG_INFO("This is a sample info message");
            SPDLOG_DEBUG("This is a sample debug message");
            SPDLOG_TRACE("This is a sample trace message");
#endif
        }
    }
}