#include "assfire/router/RouterService.hpp"
#include <grpc++/server_builder.h>
#include <cxxopts.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <assfire/log/spdlog.h>

#ifdef _WIN32
#include <assfire/win32/winsock_initializer.h>
#endif

using namespace assfire::router;
using namespace grpc;

int main(int argc, char **argv)
{
#ifdef _WIN32
    assfire::win32::winsock_initializer winsock_initializer;
#endif

    cxxopts::Options options("assfire-router-server");
    options.add_options()
            ("h,help", "Print options help")
            ("bind-address", "Service bind address", cxxopts::value<std::string>()->default_value("0.0.0.0:50051"))
            ("log-level", "Log level", cxxopts::value<std::string>()->default_value("info"))
            ("redis-cache-enabled", "Enable redis routes cache", cxxopts::value<bool>()->default_value("false"))
            ("redis-host", "Redis host address", cxxopts::value<std::string>()->default_value("127.0.0.1"))
            ("redis-port", "Redis connection port", cxxopts::value<std::size_t>()->default_value("6379"))
            ("metrics-enabled", "Prometheus metrics enabled", cxxopts::value<bool>()->default_value("false"))
            ("metrics-exposer-bind-address", "Prometheus exposer binding address", cxxopts::value<std::string>()->default_value("0.0.0.0:8081"))
            ("metrics-exposer-uri", "Prometheus exposer URI", cxxopts::value<std::string>()->default_value("/metrics"))
            ("metrics-exposer-threads-count", "Prometheus exposer threads count", cxxopts::value<std::size_t>()->default_value("1"))
            ("osrm-endpoint", "OSRM server host (where /route starts from)", cxxopts::value<std::string>()->default_value("http://router.project-osrm.org"));

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    std::string bind_address = result["bind-address"].as<std::string>();
    std::string log_level = result["log-level"].as<std::string>();

    bool enable_redis_cache = result["redis-cache-enabled"].as<bool>();
    std::string redis_host = result["redis-host"].as<std::string>();
    std::size_t redis_port = result["redis-port"].as<std::size_t>();

    bool metrics_enabled = result["metrics-enabled"].as<bool>();
    std::string metrics_exposer_bind_address = result["metrics-exposer-bind-address"].as<std::string>();
    std::string metrics_exposer_uri = result["metrics-exposer-uri"].as<std::string>();
    std::size_t metrics_exposer_threads_count = result["metrics-exposer-threads-count"].as<std::size_t>();

    std::string osrm_endpoint = result["osrm-endpoint"].as<std::string>();

    SPDLOG_INFO("Creating routing service with options: \n\
                log-level={}, \n\
				bind-address={}, \n\
				redis-cache-enabled={}, \n\
				redis-host={}, \n\
				redis-port={}, \n\
				metrics-enabled={}, \n\
				metrics-exposer-bind-address={} \n\
				metrics-exposer-uri={} \n\
				metrics-exposer-threads-count={} \n\
	            osrm-endpoint={}",
                log_level,
                bind_address,
                enable_redis_cache,
                redis_host,
                redis_port,
                metrics_enabled,
                metrics_exposer_bind_address,
                metrics_exposer_uri,
                metrics_exposer_threads_count,
                osrm_endpoint);

    assfire::log::initializeSpdlog(log_level);

    RouterService::Options routing_server_options;
    routing_server_options.use_redis = enable_redis_cache;
    routing_server_options.redis_host = redis_host;
    routing_server_options.redis_port = redis_port;

    routing_server_options.osrm_endpoint = osrm_endpoint;

    RouterService service(routing_server_options);

    ServerBuilder serverBuilder;
    serverBuilder.AddListeningPort(bind_address, grpc::InsecureServerCredentials());
    serverBuilder.RegisterService(&service);

    SPDLOG_INFO("Creating gRPC server...");

    std::unique_ptr<Server> server(serverBuilder.BuildAndStart());

    SPDLOG_INFO("Starting server on {}...", bind_address);

    server->Wait();
    return 0;
}