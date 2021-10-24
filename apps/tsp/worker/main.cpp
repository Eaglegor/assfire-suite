#include <assfire/api/v1/tsp/worker.pb.h>
#include <assfire/tsp/engine/TspSolverEngine.hpp>
#include <assfire/router/client/RouterClient.hpp>
#include <memory>
#include <cxxopts.hpp>
#include <spdlog/spdlog.h>
#include <assfire/log/spdlog.h>
#include <assfire/tsp/worker/RabbitMqTaskProcessor.hpp>
#include <assfire/tsp/worker/StdoutSolutionPublisher.hpp>
#include <assfire/tsp/worker/RedisSolutionPublisher.hpp>
#include <assfire/tsp/worker/NopSavedStateManager.hpp>
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
            (REDIS_PORT, "Redis connection port", cxxopts::value<std::size_t>()->default_value("6379"));

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

    try {

        std::unique_ptr<assfire::router::RouterApi> router =
                std::make_unique<assfire::router::RouterClient>(router_host, router_port, use_ssl_for_router);

        std::unique_ptr<assfire::tsp::SolutionPublisher> solution_publisher =
//                std::make_unique<assfire::tsp::StdoutSolutionPublisher>();
                std::make_unique<assfire::tsp::RedisSolutionPublisher>(redis_host, redis_port);

        std::unique_ptr<assfire::tsp::TspSolverEngine> tsp_solver =
                std::make_unique<assfire::tsp::TspSolverEngine>(std::move(router));

        std::unique_ptr<assfire::tsp::SavedStateManager> saved_state_manager =
                std::make_unique<assfire::tsp::NopSavedStateManager>();

        std::unique_ptr<assfire::tsp::TaskProcessor> task_processor =
                std::make_unique<assfire::tsp::RabbitMqTaskProcessor>(std::move(tsp_solver), std::move(solution_publisher), std::move(saved_state_manager),
                                                                              "localhost", 5672, "guest", "guest");


        task_processor->startProcessing();
    } catch (const std::exception &e) {
        SPDLOG_ERROR("Exception occurred: {}", e.what());
        return 1;
    }


    return 0;
}