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
#include "assfire/util/redis/RedisSerializers.hpp"

using namespace std::literals::chrono_literals;

namespace assfire::tsp
{
    static const auto SOLUTION_EXPIRY_PERIOD = 1h;

    RedisSolutionPublisher::RedisSolutionPublisher(util::RedisConnector &redis_connector)
            : redis_connector(redis_connector) {}

    void RedisSolutionPublisher::publish(const std::string &task_id, const TspTask &task, const TspSolution &solution) {
        SPDLOG_DEBUG("Publishing {}tsp result (cost = {}) for task {} to Redis storage...", solution.isFinalSolution() ? "_final_ " : "", solution.getCost().getValue(), task_id);
        redis_connector.set<assfire::api::v1::tsp::TspSolution>(
                solutionKey(task_id),
                api::v1::tsp::TspSolutionTranslator::toProto(solution),
                util::RedisProtoSerializer<assfire::api::v1::tsp::TspSolution>(),
                util::RedisRetryPolicy::retryFor(5, 5s),
                solution.isFinalSolution() ? util::RedisConnector::WriteMode::SYNC : util::RedisConnector::WriteMode::ASYNC,
                SOLUTION_EXPIRY_PERIOD
        );
    }
}