#include "assfire/scheduler/transport/SchedulerService.hpp"
#include <grpc++/server_builder.h>
#include <cxxopts.hpp>
#include <memory>
#include <numeric>
#include <spdlog/spdlog.h>
#include <assfire/log/spdlog.h>
#include <prometheus/exposer.h>

#ifdef _WIN32

#include <assfire/win32/winsock_initializer.h>

#endif

using namespace assfire::scheduler;
using namespace grpc;

int main(int argc, char **argv)
{
#ifdef _WIN32
    assfire::win32::winsock_initializer winsock_initializer;
#endif

    constexpr const char *BIND_ADDRESS = "bind-address";
    constexpr const char *LOG_LEVEL = "log-level";
    constexpr const char *METRICS_ENABLED = "metrics-enabled";
    constexpr const char *METRICS_EXPOSER_BIND_ADDRESS = "metrics-exposer-bind-address";
    constexpr const char *METRICS_EXPOSER_URI = "metrics-exposer-uri";
    constexpr const char *METRICS_EXPOSER_THREADS_COUNT = "metrics-exposer-threads-count";
    constexpr const char *ROUTER_HOST = "router-host";
    constexpr const char *ROUTER_PORT = "router-port";
    constexpr const char *ROUTER_USE_SSL = "router-use-ssl";

    cxxopts::Options args_template("assfire-tsp-server");
    args_template.add_options()
            ("h,help", "Print options help")
            (BIND_ADDRESS, "Service bind address", cxxopts::value<std::string>()->default_value("0.0.0.0:50051"))
            (LOG_LEVEL, "Log level", cxxopts::value<std::string>()->default_value("info"))
            (METRICS_ENABLED, "Prometheus metrics enabled", cxxopts::value<bool>()->default_value("false"))
            (METRICS_EXPOSER_BIND_ADDRESS, "Prometheus exposer binding address", cxxopts::value<std::string>()->default_value("0.0.0.0:8081"))
            (METRICS_EXPOSER_URI, "Prometheus exposer URI", cxxopts::value<std::string>()->default_value("/metrics"))
            (METRICS_EXPOSER_THREADS_COUNT, "Prometheus exposer threads count", cxxopts::value<std::size_t>()->default_value("1"))
            (ROUTER_HOST, "Assfire Router server host", cxxopts::value<std::string>()->default_value("http://localhost"))
            (ROUTER_PORT, "Assfire Router port to connect to", cxxopts::value<int>()->default_value("50051"))
            (ROUTER_USE_SSL, "If true, SSL will be used to connect to the Assfire Router", cxxopts::value<bool>()->default_value("false"));

    auto options = args_template.parse(argc, argv);

    if (options.count("help")) {
        std::cout << args_template.help() << std::endl;
        return 0;
    }

    std::string options_string = std::accumulate(options.arguments().begin(), options.arguments().end(), std::string(),
                                                 [](const std::string &current, const cxxopts::KeyValue &kv) {
                                                     return current + "\n" + kv.key() + " = " + kv.value();
                                                 });
    if(!options_string.empty()) {
        SPDLOG_INFO("Recognized args: {}", options_string);
    }


    assfire::log::initializeSpdlog(options[LOG_LEVEL].as<std::string>());

    SchedulerService::Options transport_scheduler_server_options;
    transport_scheduler_server_options.router_host = options[ROUTER_HOST].as<std::string>();
    transport_scheduler_server_options.router_port = options[ROUTER_PORT].as<int>();
    transport_scheduler_server_options.use_ssl_for_router = options[ROUTER_USE_SSL].as<bool>();

    if (options[METRICS_ENABLED].as<bool>()) {
        transport_scheduler_server_options.metrics_collector = MetricsCollector(std::make_shared<prometheus::Exposer>(
                options[METRICS_EXPOSER_BIND_ADDRESS].as<std::string>(),
                options[METRICS_EXPOSER_URI].as<std::string>(),
                options[METRICS_EXPOSER_THREADS_COUNT].as<std::size_t>()));
    }

    SchedulerService service(transport_scheduler_server_options);

    ServerBuilder serverBuilder;
    serverBuilder.AddListeningPort(options[BIND_ADDRESS].as<std::string>(), grpc::InsecureServerCredentials());
    serverBuilder.RegisterService(&service);

    SPDLOG_INFO("Creating gRPC server...");

    std::unique_ptr<Server> server(serverBuilder.BuildAndStart());

    SPDLOG_INFO("Starting server on {}...", options[BIND_ADDRESS].as<std::string>());

    server->Wait();
    return 0;
}