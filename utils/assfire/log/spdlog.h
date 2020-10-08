#pragma once

#include <string>
#include <spdlog/spdlog.h>

namespace assfire {
    namespace log {
        void initializeSpdlog(std::string log_level)
        {
            SPDLOG_INFO("Setting log level to {}", log_level);
            if (log_level == "critical") spdlog::set_level(spdlog::level::critical);
            else if (log_level == "err") spdlog::set_level(spdlog::level::err);
            else if (log_level == "warn") spdlog::set_level(spdlog::level::warn);
            else if (log_level == "info") spdlog::set_level(spdlog::level::info);
            else if (log_level == "debug") spdlog::set_level(spdlog::level::debug);
            else if (log_level == "trace") spdlog::set_level(spdlog::level::trace);

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