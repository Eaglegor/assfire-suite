#include <assfire/api/v1/tsp/worker.pb.h>
#include <assfire/tsp/engine/TspSolverEngine.hpp>
#include <assfire/router/client/RouterClient.hpp>
#include <memory>
#include <cxxopts.hpp>
#include <spdlog/spdlog.h>
#include <assfire/log/spdlog.h>
#include <assfire/tsp/worker/impl/RabbitMqTaskProcessor.hpp>
#include <assfire/tsp/worker/impl/StdoutSolutionPublisher.hpp>
#include <assfire/tsp/worker/impl/RedisSolutionPublisher.hpp>
#include <assfire/tsp/worker/impl/NopSavedStateManager.hpp>
#include <assfire/tsp/worker/impl/AmqpInterruptListener.hpp>
#include <assfire/tsp/worker/impl/AmqpStatusPublisher.hpp>
#include <assfire/tsp/worker/impl/AmqpTaskQueueListener.hpp>
#include <assfire/tsp/worker/impl/RedisTaskProvider.hpp>
#include <assfire/tsp/worker/Worker.hpp>

#include <cpp_redis/core/client.hpp>
#include <numeric>

#ifdef _WIN32

#include <assfire/win32/winsock_initializer.h>

#endif

using namespace assfire::tsp;

int main(int argc, char **argv) {
#ifdef _WIN32
    assfire::win32::winsock_initializer winsock_initializer;
#endif

    constexpr const char *LOG_LEVEL = "log-level";
    constexpr const char *METRICS_ENABLED = "metrics-enabled";
    constexpr const char *METRICS_EXPOSER_BIND_ADDRESS = "metrics-exposer-bind-address";
    constexpr const char *METRICS_EXPOSER_URI = "metrics-exposer-uri";
    constexpr const char *METRICS_EXPOSER_THREADS_COUNT = "metrics-exposer-threads-count";
    constexpr const char *ROUTER_HOST = "router-host";
    constexpr const char *ROUTER_PORT = "router-port";
    constexpr const char *ROUTER_USE_SSL = "router-use-ssl";
    constexpr const char *REDIS_HOST = "redis-host";
    constexpr const char *REDIS_PORT = "redis-port";
    constexpr const char *AMQP_HOST = "amqp-host";
    constexpr const char *AMQP_PORT = "amqp-port";
    constexpr const char *AMQP_LOGIN = "amqp-login";
    constexpr const char *AMQP_PASSWORD = "amqp-password";

    cxxopts::Options args_template("assfire-tsp-worker");
    args_template.add_options()
            ("h,help", "Print options help")
            (LOG_LEVEL, "Log level", cxxopts::value<std::string>()->default_value("info"))
            (METRICS_ENABLED, "Prometheus metrics enabled", cxxopts::value<bool>()->default_value("false"))
            (METRICS_EXPOSER_BIND_ADDRESS, "Prometheus exposer binding address", cxxopts::value<std::string>()->default_value("0.0.0.0:8081"))
            (METRICS_EXPOSER_URI, "Prometheus exposer URI", cxxopts::value<std::string>()->default_value("/metrics"))
            (METRICS_EXPOSER_THREADS_COUNT, "Prometheus exposer threads count", cxxopts::value<std::size_t>()->default_value("1"))
            (ROUTER_HOST, "Assfire Router server host", cxxopts::value<std::string>()->default_value("http://localhost"))
            (ROUTER_PORT, "Assfire Router port to connect to", cxxopts::value<int>()->default_value("50051"))
            (ROUTER_USE_SSL, "If true, SSL will be used to connect to the Assfire Router", cxxopts::value<bool>()->default_value("false"))
            (REDIS_HOST, "Redis host address", cxxopts::value<std::string>()->default_value("127.0.0.1"))
            (REDIS_PORT, "Redis connection port", cxxopts::value<std::size_t>()->default_value("6379"))
            (AMQP_HOST, "AMQP/RabbitMQ host address", cxxopts::value<std::string>()->default_value("127.0.0.1"))
            (AMQP_PORT, "AMQP/RabbitMQ connection port", cxxopts::value<std::size_t>()->default_value("5672"))
            (AMQP_LOGIN, "AMQP/RabbitMQ login", cxxopts::value<std::string>()->default_value("guest"))
            (AMQP_PASSWORD, "AMQP/RabbitMQ password", cxxopts::value<std::string>()->default_value("guest"));

    auto options = args_template.parse(argc, argv);

    if (options.count("help")) {
        std::cout << args_template.help() << std::endl;
        return 0;
    }

    std::string options_string = std::accumulate(options.arguments().begin(), options.arguments().end(), std::string(),
                                                 [](const std::string &current, const cxxopts::KeyValue &kv) {
                                                     return current + "\n" + kv.key() + " = " + kv.value();
                                                 });
    if (!options_string.empty()) {
        SPDLOG_INFO("Recognized args: {}", options_string);
    }

    assfire::log::initializeSpdlog(options[LOG_LEVEL].as<std::string>());

    std::string router_host = options[ROUTER_HOST].as<std::string>();
    int router_port = options[ROUTER_PORT].as<int>();
    bool use_ssl_for_router = options[ROUTER_USE_SSL].as<bool>();

    std::string redis_host = options[REDIS_HOST].as<std::string>();
    std::size_t redis_port = options[REDIS_PORT].as<std::size_t>();
    std::string amqp_host = options[AMQP_HOST].as<std::string>();
    std::size_t amqp_port = options[AMQP_PORT].as<std::size_t>();
    std::string amqp_login = options[AMQP_LOGIN].as<std::string>();
    std::string amqp_password = options[AMQP_PASSWORD].as<std::string>();

    try {
        SPDLOG_INFO("Creating router client");
        std::unique_ptr<assfire::router::RouterApi> router =
                std::make_unique<assfire::router::RouterClient>(router_host, router_port, use_ssl_for_router);

        SPDLOG_INFO("Creating solution publisher");
        std::unique_ptr<assfire::tsp::SolutionPublisher> solution_publisher =
//                std::make_unique<assfire::tsp::StdoutSolutionPublisher>();
                std::make_unique<assfire::tsp::RedisSolutionPublisher>(redis_host, redis_port);

        SPDLOG_INFO("Creating TSP solver");
        std::unique_ptr<assfire::tsp::TspSolverEngine> tsp_solver =
                std::make_unique<assfire::tsp::TspSolverEngine>(std::move(router));

        SPDLOG_INFO("Creating TSP saved state manager");
        std::unique_ptr<assfire::tsp::SavedStateManager> saved_state_manager =
                std::make_unique<assfire::tsp::NopSavedStateManager>();

        SPDLOG_INFO("Creating redis client for TSP task provider");
        std::unique_ptr<cpp_redis::client> redis_client =
                std::make_unique<cpp_redis::client>();

        SPDLOG_INFO("Redis client connecting to {}:{}...", redis_host, redis_port);
        redis_client->connect(redis_host, redis_port, [](const std::string &host, std::size_t port, cpp_redis::client::connect_state status) {
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
            SPDLOG_INFO("Redis connection state (task provider) for {}:{} has changed to {}", host, port, string_status);
        });

        SPDLOG_INFO("Creating TSP task provider");
        std::unique_ptr<assfire::tsp::TaskProvider> task_provider =
                std::make_unique<assfire::tsp::RedisTaskProvider>(std::move(redis_client));

        SPDLOG_INFO("Creating AMQP connector for task queue listener");
        std::unique_ptr<RabbitMqConnector> amqp_connector =
                std::make_unique<RabbitMqConnector>("TaskQueueListener");

        SPDLOG_INFO("Connecting AMQP connector for task queue listener to RabbitMQ server");
        amqp_connector->connect(amqp_host, amqp_port, amqp_login, amqp_password);

        SPDLOG_INFO("Connecting task queue listener");
        std::unique_ptr<assfire::tsp::TaskQueueListener> task_queue_listener =
                std::make_unique<assfire::tsp::AmqpTaskQueueListener>(std::move(amqp_connector));

        SPDLOG_INFO("Creating AMQP connector for status publisher");
        amqp_connector = std::make_unique<RabbitMqConnector>("StatusPublisher");

        SPDLOG_INFO("Connecting AMQP connector for status publisher to RabbitMQ server");
        amqp_connector->connect(amqp_host, amqp_port, amqp_login, amqp_password);

        SPDLOG_INFO("Creating status publisher");
        std::unique_ptr<assfire::tsp::StatusPublisher> status_publisher =
                std::make_unique<assfire::tsp::AmqpStatusPublisher>(std::move(amqp_connector));

        SPDLOG_INFO("Creating AMQP connector for interrupt listener");
        amqp_connector = std::make_unique<RabbitMqConnector>("InterruptListener");

        SPDLOG_INFO("Connecting AMQP connector for interrupt listener to RabbitMQ server");
        amqp_connector->connect(amqp_host, amqp_port, amqp_login, amqp_password);

        SPDLOG_INFO("Creating interrupt listener");
        std::unique_ptr<assfire::tsp::InterruptListener> interrupt_listener =
                std::make_unique<assfire::tsp::AmqpInterruptListener>(std::move(amqp_connector));

        SPDLOG_INFO("Creating worker");
        std::unique_ptr<assfire::tsp::Worker> worker =
                std::make_unique<assfire::tsp::Worker>(std::move(task_queue_listener),
                                                       std::move(interrupt_listener),
                                                       std::move(task_provider),
                                                       std::move(status_publisher),
                                                       std::move(saved_state_manager),
                                                       std::move(solution_publisher),
                                                       std::move(tsp_solver)
                );

        SPDLOG_INFO("Starting worker");
        worker->start();
    } catch (const std::exception &e) {
        SPDLOG_ERROR("Unhandled exception: {}", e.what());
        return 1;
    }


    return 0;
}