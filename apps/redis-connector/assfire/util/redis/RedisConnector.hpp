#pragma once

#include <cpp_redis/cpp_redis>
#include <functional>
#include <atomic>
#include <optional>
#include <mutex>
#include <condition_variable>
#include <spdlog/spdlog.h>

#include "RedisConnectionOpts.hpp"
#include "RedisError.hpp"
#include "RedisRetryPolicy.hpp"

namespace assfire::util
{
    class RedisConnector
    {
    public:
        template<typename T>
        using ReplyParser = std::function<T(const cpp_redis::reply &)>;

        template<typename T, typename R>
        using Serializer = std::function<R(const T &)>;

        static RedisRetryPolicy NO_RETRY;

        enum class WriteMode
        {
            SYNC,
            ASYNC
        };

        RedisConnector(RedisConnectionOpts options);

        template<typename T>
        std::future<std::optional<T>> get(const std::string &key, const ReplyParser<T> &parse, const RedisRetryPolicy &retry_policy = NO_RETRY) {
            SPDLOG_TRACE("GET for object with key {}", key);
            return std::async(std::launch::async, [this, key, parse, retry_policy] {
                return performWithReconnect<std::optional<T>>([&](cpp_redis::client &client) -> std::optional<T> {
                    auto freply = client.get(key);
                    client.sync_commit();
                    auto reply = freply.get();

                    if (reply.is_error()) {
                        throw redis_exception(RedisError::fromReply(reply));
                    }
                    if (reply.is_null()) {
                        return std::nullopt;
                    }
                    return parse(reply);
                }, retry_policy);
            });
        }

        template<typename T>
        void set(
                const std::string &key,
                const T &obj,
                const Serializer<T, std::string> &serialize,
                const RedisRetryPolicy &retry_policy = NO_RETRY,
                WriteMode write_mode = WriteMode::SYNC,
                std::chrono::seconds expire = std::chrono::seconds(0)) {
            SPDLOG_TRACE("SET for object with key {} in sync mode", key);
            performWithReconnect<void>([&](cpp_redis::client &client) {
                auto freply = client.set_advanced(key, serialize(obj), expire.count() > 0, expire.count());
                client.sync_commit();
                auto reply = freply.get();

                if (reply.is_error()) {
                    throw redis_exception(RedisError::fromReply(reply));
                }
            }, retry_policy);
        }

        void del(const std::string &key, const RedisRetryPolicy &retry_policy = NO_RETRY, WriteMode write_mode = WriteMode::SYNC) {
            SPDLOG_TRACE("DEL for object with key {} in sync mode", key);
            performWithReconnect<void>([&](cpp_redis::client &client) {
                auto freply = client.del({key});
                client.sync_commit();
                auto reply = freply.get();

                if (reply.is_error()) {
                    throw redis_exception(RedisError::fromReply(reply));
                }
            }, retry_policy);
        }

        int incr(const std::string &key, const RedisRetryPolicy &retry_policy = NO_RETRY, WriteMode write_mode = WriteMode::SYNC) {
            SPDLOG_TRACE("INCR for key {} in sync mode", key);
            return performWithReconnect<int>([&](cpp_redis::client &client) {
                auto freply = client.incr(key);
                client.sync_commit();
                auto reply = freply.get();

                if (reply.is_error()) {
                    throw redis_exception(RedisError::fromReply(reply));
                }
                return reply.as_integer();
            }, retry_policy);
        }

        template<typename T>
        std::future<std::optional<T>> getset(const std::string &key, const T &new_value, const ReplyParser<T> &parse, const Serializer<T, std::string> &serialize, const RedisRetryPolicy &retry_policy = NO_RETRY) {
            SPDLOG_TRACE("GETSET for object with key {}", key);
            return std::async(std::launch::async, [this, key, new_value, parse, serialize, retry_policy] {
                return performWithReconnect<std::optional<T>>([&](cpp_redis::client &client) -> std::optional<T> {
                    auto freply = client.getset(key, serialize(new_value));
                    client.sync_commit();
                    auto reply = freply.get();

                    if (reply.is_error()) {
                        throw redis_exception(RedisError::fromReply(reply));
                    }
                    if (reply.is_null()) {
                        return std::nullopt;
                    }
                    return parse(reply);
                }, retry_policy);
            });
        }

        void connect(const RedisRetryPolicy &retry_policy);

    private:
        template<typename T>
        using Action = std::function<T(cpp_redis::client &client)>;

        template<typename T>
        T performWithReconnect(const Action<T> &action, const RedisRetryPolicy &retry_policy) {
            for (int i = 0; ; ++i) {
                try {
                    if (!redis_client.is_connected()) {
                        std::lock_guard<std::mutex> lck(connect_mtx);
                        if (!redis_client.is_connected()) {
                            connect(NO_RETRY);
                        }
                    }

                    return action(redis_client);
                } catch (const redis_exception &e) {
                    if (!e.getError().isRecoverable() || !retry_policy.attemptAllowed(i + 1)) {
                        SPDLOG_ERROR("There was an {}error while performing redis action on attempt {}: {}", (e.getError().isRecoverable() ? "" : "unrecoverable "), i, e.what());
                        throw e;
                    } else {
                        SPDLOG_ERROR("There was an error while performing redis action on attempt {}: {}. Will retry in {} ms", i, e.what(), retry_policy.getDelayMs(i).count());
                        std::this_thread::sleep_for(retry_policy.getDelayMs(i));
                    }
                }
            }
        }

        RedisConnectionOpts options;

        cpp_redis::client redis_client;
        std::mutex connect_mtx;
    };
}