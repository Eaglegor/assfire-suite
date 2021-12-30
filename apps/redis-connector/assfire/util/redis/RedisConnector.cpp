#include "RedisConnector.hpp"
#include <spdlog/spdlog.h>
#include <chrono>
#include <thread>

using namespace std::literals::chrono_literals;

namespace assfire::util
{
    RedisRetryPolicy RedisConnector::NO_RETRY = RedisRetryPolicy::noRetry();

    RedisConnector::RedisConnector(RedisConnectionOpts options)
            : options(std::move(options)) {

    }

//    std::optional<std::string> RedisConnector::get(const std::string &key) const {
//        std::optional<std::string> result;
//        performWithReconnect([&](cpp_redis::client &client) {
//            auto freply = client.get(key);
//            client.sync_commit();
//            auto reply = freply.get();
//
//            if (reply.is_error()) {
//                throw redis_exception(RedisError::fromReply(reply));
//            }
//            if (!reply.is_string()) {
//                throw redis_exception(RedisError(RedisErrorType::INVALID_FORMAT, "Invalid format (expected string)"));
//            } else {
//                result = reply.as_string();
//            }
//        });
//        return result;
//    }


    void RedisConnector::connect(const RedisRetryPolicy &retry_policy) {
        for (int i = 0; !redis_client.is_connected() && retry_policy.attemptAllowed(i); ++i) {
            try {
                redis_client.connect(options.host, options.port, [&](const std::string &host, std::size_t port, cpp_redis::client::connect_state status) {
                    std::string string_status;
                    switch (status) {
                        case cpp_redis::client::connect_state::dropped:
                            string_status = "DROPPED";
                            break;
                        case cpp_redis::client::connect_state::start:
                            string_status = "START";
                            break;
                        case cpp_redis::client::connect_state::sleeping:
                            string_status = "SLEEPING";
                            break;
                        case cpp_redis::client::connect_state::ok:
                            string_status = "OK";
                            break;
                        case cpp_redis::client::connect_state::failed:
                            string_status = "FAILED";
                            break;
                        case cpp_redis::client::connect_state::lookup_failed:
                            string_status = "LOOKUP_FAILED";
                            break;
                        case cpp_redis::client::connect_state::stopped:
                            string_status = "STOPPED";
                            break;
                        default:
                            string_status = "UNKNOWN";
                            break;
                    }
                    SPDLOG_INFO("Redis connection state for {}:{} has changed to {}", host, port, string_status);
                });
            } catch (const std::exception &e) {
                if (retry_policy.attemptAllowed(i + 1)) {
                    SPDLOG_ERROR("Failed to connect to redis at {}:{} at attempt {}. Will retry in {} ms", options.host, options.port, i, retry_policy.getDelayMs(i).count());
                    std::this_thread::sleep_for(retry_policy.getDelayMs(i));
                } else {
                    SPDLOG_ERROR("Failed to connect to redis at {}:{} at attempt {}", options.host, options.port, i);
                    throw redis_exception(RedisError(RedisErrorType::UNKNOWN, e.what()));
                }
            }
        }
    }
}