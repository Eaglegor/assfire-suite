#include "RedisSolutionStorage.hpp"
#include <cpp_redis/core/client.hpp>
#include "assfire/tsp/TspRedisConstants.hpp"
#include "assfire/api/v1/tsp/worker.pb.h"
#include <spdlog/spdlog.h>
#include <thread>
#include <atomic>
#include <future>

namespace assfire::tsp {

    namespace {
        std::string formatKey(const std::string &task_id) {
            return std::string(TSP_REDIS_WORKER_KEY_PREFIX) + task_id + std::string(TSP_REDIS_WORKER_SOLUTION_KEY_SUFFIX);
        }
    }

    std::optional<SolutionStorage::Solution> RedisSolutionStorage::fetchSolution(const std::string &task_id) const {
        std::future<cpp_redis::reply> freply = redis_client->get(formatKey(task_id));
        redis_client->sync_commit();

        cpp_redis::reply reply = freply.get();
        if (reply.is_error()) {
            SPDLOG_ERROR("Error while trying to retrieve solution {} from cache: {}", task_id, reply.error());
            return std::nullopt;
        } else if (!reply.is_string()) {
            SPDLOG_INFO("Solution for task {} not found in cache: {}", task_id);
            return std::nullopt;
        }

        assfire::api::v1::tsp::TspSolutionResult result;
        result.ParseFromString(reply.as_string());

        if(result.is_success()) {
            return result.solution();
        } else {
            throw std::runtime_error("Stored solution has unsuccessful state. Looks like an error occurred while calculation");
        }
    }
}
