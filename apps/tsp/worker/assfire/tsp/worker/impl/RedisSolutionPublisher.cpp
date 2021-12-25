#include "RedisSolutionPublisher.hpp"
#include <spdlog/spdlog.h>
#include <cpp_redis/core/reply.hpp>
#include <cpp_redis/core/client.hpp>
#include <assfire/api/v1/tsp/translators/TspSolutionTranslator.hpp>
#include <assfire/api/v1/tsp/worker.pb.h>
#include <assfire/tsp/worker/impl/TspWorkerConstants.hpp>
#include <assfire/tsp/worker/impl/TspWorkerKeys.hpp>
#include "RedisConnectionCallback.hpp"
#include <future>

namespace assfire::tsp {

    namespace {
        const int SOLUTION_EXPIRY_PERIOD_SECONDS = 3600;

        void publishResult(cpp_redis::client &client, const std::string &task_id, const assfire::api::v1::tsp::TspSolution &solution) {
            try {
                std::string key = solutionKey(task_id);
                client.set(key, solution.SerializeAsString(), [](const cpp_redis::reply &rpl) {
                    if (rpl.is_error()) {
                        SPDLOG_ERROR("Couldn't save TSP result to Redis storage: {}", rpl.error());
                    }
                });
                client.expire(key, SOLUTION_EXPIRY_PERIOD_SECONDS);
                client.sync_commit();
            } catch (std::exception &e) {
                SPDLOG_ERROR("Couldn't createConnection to Redis storage to save results: {}", e.what());
            }
        }
    }

    void RedisSolutionPublisher::publish(const std::string &task_id, const TspTask &task, const TspSolution &solution) {
        SPDLOG_DEBUG("Publishing {}tsp result (cost = {}) for task {} to Redis storage...", solution.isFinalSolution() ? "_final_ " : "", solution.getCost().getValue(), task_id);
        publishResult(*client, task_id, api::v1::tsp::TspSolutionTranslator::toProto(solution));
    }

    RedisSolutionPublisher::RedisSolutionPublisher(std::unique_ptr<cpp_redis::client> client) : client(std::move(client)) {}

    RedisSolutionPublisher::~RedisSolutionPublisher() {

    }
}