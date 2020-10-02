#include <grpc++/server_builder.h>
#include <cxxopts.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <assfire/log/spdlog.h>
#include <assfire/sample/Greeter.hpp>
#include <prometheus/exposer.h>
#include "GreeterService.h"

#ifdef _WIN32

#include <assfire/win32/winsock_initializer.h>

#endif

int main(int argc, char **argv)
{
#ifdef _WIN32
    assfire::win32::winsock_initializer winsock_helper;
#endif

    cxxopts::Options options("assfire-sample-server");
    options.add_options()
            ("h,help","Print options help")
            ("bind-address", "Service bind address", cxxopts::value<std::string>()->default_value("0.0.0.0:50051"))
            ("log-level", "Log level", cxxopts::value<std::string>()->default_value("info"))
            ("metrics-enabled", "Prometheus metrics enabled", cxxopts::value<bool>()->default_value("false"))
            ("metrics-exposer-bind-address", "Prometheus exposer binding address", cxxopts::value<std::string>()->default_value("0.0.0.0:8081"))
            ("metrics-exposer-uri", "Prometheus exposer URI", cxxopts::value<std::string>()->default_value("/metrics"))
            ("metrics-exposer-threads-count", "Prometheus exposer threads count", cxxopts::value<std::size_t>()->default_value("1"));

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    std::string bind_address = result["bind-address"].as<std::string>();
    std::string log_level = result["log-level"].as<std::string>();
    assfire::log::initializeSpdlog(log_level);

    bool metrics_enabled = result["metrics-enabled"].as<bool>();
    std::string metrics_exposer_bind_address = result["metrics-exposer-bind-address"].as<std::string>();
    std::string metrics_exposer_uri = result["metrics-exposer-uri"].as<std::string>();
    std::size_t metrics_exposer_threads_count = result["metrics-exposer-threads-count"].as<std::size_t>();

    SPDLOG_INFO("Creating routing service with options: \n\
				bind-address={}, \n\
				metrics-enabled={}, \n\
				metrics-exposer-bind-address={} \n\
				metrics-exposer-uri={} \n\
				metrics-exposer-threads-count={}",
                bind_address,
                metrics_enabled,
                metrics_exposer_bind_address,
                metrics_exposer_uri,
                metrics_exposer_threads_count);

    assfire::GreeterService service(metrics_enabled ?
                                    assfire::MetricsCollector(std::make_shared<prometheus::Exposer>(
                                            metrics_exposer_bind_address,
                                            metrics_exposer_uri,
                                            metrics_exposer_threads_count))
                                                    : assfire::MetricsCollector());

    grpc::ServerBuilder serverBuilder;
    serverBuilder.AddListeningPort(bind_address, grpc::InsecureServerCredentials());
    serverBuilder.RegisterService(&service);

    SPDLOG_INFO("Creating gRPC server...");

    std::unique_ptr<grpc::Server> server(serverBuilder.BuildAndStart());

    SPDLOG_INFO("Starting server on {}...", bind_address);

    server->Wait();
    return 0;
}