#include "RedisTaskProvider.hpp"
#include "TspWorkerConstants.hpp"
#include <cpp_redis/cpp_redis>
#include <spdlog/spdlog.h>
#include <chrono>
#include "assfire/api/v1/tsp/worker.pb.h"
#include "assfire/api/v1/tsp/translators/TspTaskTranslator.hpp"
#include "TspWorkerKeys.hpp"
#include "assfire/util/redis/RedisConnector.hpp"
#include "assfire/util/redis/RedisParsers.hpp"
#include "assfire/util/redis/RedisSerializers.hpp"

using namespace assfire::api::v1::tsp;
using namespace std::literals::chrono_literals;

namespace assfire::tsp
{

    static util::RedisRetryPolicy DEFAULT_RETRY_POLICY = util::RedisRetryPolicy::retryFor(5, 5s);

    RedisTaskProvider::RedisTaskProvider(util::RedisConnector &redis_connector)
            : redis_connector(redis_connector) {}

    std::optional<TspTask> RedisTaskProvider::retrieveTask(const std::string &task_id) {
        try {
            auto task = redis_connector.get<api::v1::tsp::TspTask>(
                    taskKey(task_id),
                    util::RedisProtoParser<api::v1::tsp::TspTask>(),
                    DEFAULT_RETRY_POLICY
            ).get();
            return task ? std::make_optional(api::v1::tsp::TspTaskTranslator::fromProto(*task)) : std::nullopt;
        } catch (const util::redis_exception &e) {
            SPDLOG_ERROR("Failed to retrieve task {}: {}", task_id, e.what());
            throw e;
        }
    }

    bool RedisTaskProvider::tryLock(const std::string &task_id) {
        try {
            return !redis_connector.getset<std::string>(
                    lockKey(task_id),
                    TSP_WORKER_LOCK_VALUE,
                    util::RedisStringParser(),
                    util::RedisStringSerializer(),
                    DEFAULT_RETRY_POLICY
            ).get().has_value();
        } catch (const util::redis_exception &e) {
            SPDLOG_ERROR("Failed to set lock on task {}: {}", task_id, e.what());
            throw e;
        }
    }

    void RedisTaskProvider::unlock(const std::string &task_id) {
        try {
            redis_connector.del(
                    lockKey(task_id),
                    DEFAULT_RETRY_POLICY,
                    util::RedisConnector::WriteMode::SYNC
            );
        } catch (const util::redis_exception &e) {
            SPDLOG_ERROR("Failed to clear lock for task {}: {}", task_id, e.what());
            throw e;
        }
    }

    void RedisTaskProvider::sendStarted(const std::string &task_id) {
        sendStatusSignal(task_id, WorkerTspStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_STARTED, true);
    }

    void RedisTaskProvider::sendError(const std::string &task_id) {
        sendStatusSignal(task_id, WorkerTspStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_ERROR, true);
    }

    void RedisTaskProvider::sendInProgress(const std::string &task_id) {
        sendStatusSignal(task_id, WorkerTspStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_IN_PROGRESS, false);
    }

    void RedisTaskProvider::sendPaused(const std::string &task_id) {
        sendStatusSignal(task_id, WorkerTspStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_PAUSED, true);
    }

    void RedisTaskProvider::sendInterrupted(const std::string &task_id) {
        sendStatusSignal(task_id, WorkerTspStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_INTERRUPTED, true);
    }

    void RedisTaskProvider::sendFinished(const std::string &task_id) {
        sendStatusSignal(task_id, WorkerTspStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_FINISHED, true);
    }

    bool RedisTaskProvider::isFinished(const std::string &task_id) {
        try {
            auto saved_state = redis_connector.get<std::string>(
                    statusKey(task_id),
                    util::RedisStringParser(),
                    DEFAULT_RETRY_POLICY
            ).get();
            return saved_state && *saved_state == WorkerTspStatusUpdate_Type_Name(WorkerTspStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_FINISHED);
        } catch (const util::redis_exception &e) {
            SPDLOG_ERROR("Failed to determine if task {} is finished: {}", task_id, e.what());
            throw e;
        }
    }

    bool RedisTaskProvider::isPaused(const std::string &task_id) {
        try {
            auto saved_state = redis_connector.get<std::string>(
                    statusKey(task_id),
                    util::RedisStringParser(),
                    DEFAULT_RETRY_POLICY
            ).get();
            return saved_state && *saved_state == WorkerTspStatusUpdate_Type_Name(WorkerTspStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_PAUSED);
        } catch (const util::redis_exception &e) {
            SPDLOG_ERROR("Failed to determine if task {} is finished: {}", task_id, e.what());
            throw e;
        }
    }

    void RedisTaskProvider::sendStatusSignal(const std::string &task_id, api::v1::tsp::WorkerTspStatusUpdate_Type signal, bool sync) {
        try {
            redis_connector.set<std::string>(
                    statusKey(task_id),
                    WorkerTspStatusUpdate_Type_Name(signal),
                    util::RedisStringSerializer(),
                    DEFAULT_RETRY_POLICY,
                    sync ? util::RedisConnector::WriteMode::SYNC : util::RedisConnector::WriteMode::ASYNC
            );
        } catch (const util::redis_exception &e) {
            SPDLOG_ERROR("Failed to write current status for task {}: {}", task_id, e.what());
            throw e;
        }

        redis_connector.set<std::string>(
                keepAliveKey(task_id),
                TSP_WORKER_KEEPALIVE_VALUE,
                util::RedisStringSerializer(),
                DEFAULT_RETRY_POLICY,
                util::RedisConnector::WriteMode::ASYNC,
                TSP_WORKER_KEEPALIVE_EXPIRY_SEC
        );
    }

    int RedisTaskProvider::incAttempts(const std::string &task_id) {
        try {
            return redis_connector.incr(
                    attemptsKey(task_id),
                    DEFAULT_RETRY_POLICY,
                    util::RedisConnector::WriteMode::SYNC
            );
        } catch (const util::redis_exception &e) {
            SPDLOG_ERROR("Failed to increment attempts count for task {}: {}", task_id, e.what());
            throw e;
        }
    }
}
