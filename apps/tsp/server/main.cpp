#include "assfire/tsp/TspService.hpp"
#include <grpc++/server_builder.h>
#include <cxxopts.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <assfire/log/spdlog.h>
#include "assfire/tsp/RabbitMQWorkerTransport.hpp"
#include "assfire/tsp/RedisSolutionStorage.hpp"
#include "assfire/tsp/IncrementalTaskIdGenerator.hpp"
#include "assfire/tsp/NopTspTaskStorage.hpp"
#include "assfire/tsp/RedisTspTaskStorage.hpp"
#include "cpp_redis/core/client.hpp"
#include <numeric>

#ifdef _WIN32

#include <assfire/win32/winsock_initializer.h>

#endif

using namespace assfire::tsp;
using namespace grpc;

int main(int argc, char **argv) {
#ifdef _WIN32
    assfire::win32::winsock_initializer winsock_initializer;
#endif

    constexpr const char *LOG_LEVEL = "log-level";
    constexpr const char *BIND_ADDRESS = "bind-address";
    constexpr const char *METRICS_ENABLED = "metrics-enabled";
    constexpr const char *METRICS_EXPOSER_BIND_ADDRESS = "metrics-exposer-bind-address";
    constexpr const char *METRICS_EXPOSER_URI = "metrics-exposer-uri";
    constexpr const char *METRICS_EXPOSER_THREADS_COUNT = "metrics-exposer-threads-count";
    constexpr const char *REDIS_HOST = "redis-host";
    constexpr const char *REDIS_PORT = "redis-port";

    cxxopts::Options args_template("assfire-tsp-server");
    args_template.add_options()
            ("h,help", "Print options help")
            (BIND_ADDRESS, "Service bind address", cxxopts::value<std::string>()->default_value("0.0.0.0:50051"))
            (LOG_LEVEL, "Log level", cxxopts::value<std::string>()->default_value("info"))
            (METRICS_ENABLED, "Prometheus metrics enabled", cxxopts::value<bool>()->default_value("false"))
            (METRICS_EXPOSER_BIND_ADDRESS, "Prometheus exposer binding address", cxxopts::value<std::string>()->default_value("0.0.0.0:8081"))
            (METRICS_EXPOSER_URI, "Prometheus exposer URI", cxxopts::value<std::string>()->default_value("/metrics"))
            (METRICS_EXPOSER_THREADS_COUNT, "Prometheus exposer threads count", cxxopts::value<std::size_t>()->default_value("1"))
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

    std::string bind_address = options[BIND_ADDRESS].as<std::string>();
    std::string log_level = options[LOG_LEVEL].as<std::string>();

    bool metrics_enabled = options[METRICS_ENABLED].as<bool>();
    std::string metrics_exposer_bind_address = options[METRICS_EXPOSER_BIND_ADDRESS].as<std::string>();
    std::string metrics_exposer_uri = options[METRICS_EXPOSER_URI].as<std::string>();
    std::size_t metrics_exposer_threads_count = options[METRICS_EXPOSER_THREADS_COUNT].as<std::size_t>();

    try {
        std::shared_ptr<cpp_redis::client> redis_client = std::make_shared<cpp_redis::client>();
        redis_client->connect(options[REDIS_HOST].as<std::string>(), options[REDIS_PORT].as<std::size_t>());

        TspService service(
                std::make_unique<RabbitMQWorkerTransport>("localhost", 5672, "guest", "guest"),
                std::make_unique<RedisSolutionStorage>(),
                std::make_unique<RedisTspTaskStorage>(redis_client),
                std::make_unique<IncrementalTaskIdGenerator>());

        ServerBuilder serverBuilder;
        serverBuilder.AddListeningPort(bind_address, grpc::InsecureServerCredentials());
        serverBuilder.RegisterService(&service);

        SPDLOG_INFO("Creating gRPC server...");

        std::unique_ptr<Server> server(serverBuilder.BuildAndStart());

        SPDLOG_INFO("Starting server on {}...", bind_address);

        server->Wait();
    } catch (const std::exception &e) {
        SPDLOG_ERROR("Exception thrown in service: {}", e.what());
        return 1;
    }


    return 0;
}