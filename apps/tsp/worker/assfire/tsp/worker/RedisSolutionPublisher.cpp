#include "RedisSolutionPublisher.hpp"
#include <spdlog/spdlog.h>
#include <cpp_redis/core/reply.hpp>
#include <cpp_redis/core/client.hpp>
#include <assfire/api/v1/tsp/translators/TspSolutionTranslator.hpp>
#include <assfire/api/v1/tsp/worker.pb.h>
#include <future>

namespace assfire::tsp {

    RedisSolutionPublisher::RedisSolutionPublisher(const std::string &redis_host, std::size_t redis_port) {
        SPDLOG_INFO("Initializing Redis TSP solution publisher... Creating Redis client");
        client = std::make_unique<cpp_redis::client>();
        SPDLOG_INFO("Redis client connecting to {}:{}...", redis_host, redis_port);
        client->connect(redis_host, redis_port, [](const std::string &host, std::size_t port, cpp_redis::client::connect_state status) {
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
        SPDLOG_INFO("Redis TSP solution publisher initialized");
    }

    namespace {
        std::string formatKey(const std::string &task_id) {
            return "asf.tsp.sol|" + task_id;
        }

        void publishResult(cpp_redis::client &client, const std::string &task_id, const assfire::api::v1::tsp::TspSolutionResult &result) {
            try {
                client.set(formatKey(task_id), result.SerializeAsString(), [](const cpp_redis::reply &rpl) {
                    if (rpl.is_error()) {
                        SPDLOG_ERROR("Couldn't save TSP result to Redis storage: {}", rpl.error());
                    }
                });
                client.sync_commit();
            } catch (std::exception &e) {
                SPDLOG_ERROR("Couldn't connect to Redis storage to save results: {}", e.what());
            }
        }
    }

    void RedisSolutionPublisher::publish(const std::string &task_id, const TspTask &task, const TspSolution &solution) {
        SPDLOG_INFO("Publishing {}tsp result (cost = {}) for task {} to Redis storage...", solution.isFinalSolution() ? "_final_ " : "", solution.getCost().getValue(), task_id);
        assfire::api::v1::tsp::TspSolutionResult result;
        result.set_is_success(true);
        result.mutable_solution()->CopyFrom(assfire::api::v1::tsp::TspSolutionTranslator::toProto(solution));
        publishResult(*client, task_id, result);
    }

    void RedisSolutionPublisher::onError(const std::string &task_id) {
        SPDLOG_INFO("Publishing error for task {} to Redis storage...", task_id);
        assfire::api::v1::tsp::TspSolutionResult result;
        result.set_is_success(false);
        publishResult(*client, task_id, result);
    }
}