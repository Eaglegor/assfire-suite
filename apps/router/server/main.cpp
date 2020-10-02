#include <assfire/routing/v1/routing.pb.h>
#include "../engine/RoutingService.hpp"
#include <grpc++/server_builder.h>
#include <cxxopts.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#ifdef _WIN32
#include "WinSockHelper.hpp"
#endif

using namespace assfire;
using namespace assfire::routing::proto::v1;
using namespace grpc;

namespace
{
	void initLogger(std::string level)
	{
		SPDLOG_INFO("Setting log level to {}", level);
		if (level == "critical") spdlog::set_level(spdlog::level::critical);
		else if (level == "err") spdlog::set_level(spdlog::level::err);
		else if (level == "warn") spdlog::set_level(spdlog::level::warn);
		else if (level == "info") spdlog::set_level(spdlog::level::info);
		else if (level == "debug") spdlog::set_level(spdlog::level::debug);
		else if (level == "trace") spdlog::set_level(spdlog::level::trace);

#ifdef ASSFIRE_DEBUG_MODE
		SPDLOG_CRITICAL("This is a test critical message");
		SPDLOG_ERROR("This is a test err message");
		SPDLOG_WARN("This is a test warn message");
		SPDLOG_INFO("This is a test info message");
		SPDLOG_DEBUG("This is a test debug message");
		SPDLOG_TRACE("This is a test trace message");
#endif
	}
}

int main(int argc, char** argv)
{
#ifdef _WIN32
	WinSockHelper winsock_helper;
#endif

	cxxopts::Options options("routes_provider_server");
	options.add_options()
		("h,help","Print options help")
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

	if (result.count("help"))
	{
		std::cout << options.help() << std::endl;
		return 0;
	}

	std::string bind_address = result["bind-address"].as<std::string>();
	std::string log_level = result["log-level"].as<std::string>();
	initLogger(log_level);

	bool enable_redis_cache = result["redis-cache-enabled"].as<bool>();
	std::string redis_host = result["redis-host"].as<std::string>();
	std::size_t redis_port = result["redis-port"].as<std::size_t>();

	bool metrics_enabled = result["metrics-enabled"].as<bool>();
	std::string metrics_exposer_bind_address = result["metrics-exposer-bind-address"].as<std::string>();
	std::string metrics_exposer_uri = result["metrics-exposer-uri"].as<std::string>();
	std::size_t metrics_exposer_threads_count = result["metrics-exposer-threads-count"].as<std::size_t>();

	std::string osrm_endpoint = result["osrm-endpoint"].as<std::string>();

	SPDLOG_INFO("Creating routing service with options: \n\
				bind-address={}, \n\
				redis-cache-enabled={}, \n\
				redis-host={}, \n\
				redis-port={}, \n\
				metrics-enabled={}, \n\
				metrics-exposer-bind-address={} \n\
				metrics-exposer-uri={} \n\
				metrics-exposer-threads-count={} \n\
	            osrm-endpoint={}",
		bind_address,
		enable_redis_cache, 
		redis_host, 
		redis_port, 
		metrics_enabled,
		metrics_exposer_bind_address,
		metrics_exposer_uri,
		metrics_exposer_threads_count,
		osrm_endpoint);

	assfire::RoutingService service(enable_redis_cache, redis_host, redis_port, 
		metrics_enabled ? 
		RoutingMetricsCollector(std::make_shared<prometheus::Exposer>(
			metrics_exposer_bind_address, 
			metrics_exposer_uri, 
			metrics_exposer_threads_count)) 
		: RoutingMetricsCollector(),
		osrm_endpoint
	);

	ServerBuilder serverBuilder;
	serverBuilder.AddListeningPort(bind_address, grpc::InsecureServerCredentials());
	serverBuilder.RegisterService(&service);

	SPDLOG_INFO("Creating gRPC server...");

	std::unique_ptr<Server> server(serverBuilder.BuildAndStart());

	SPDLOG_INFO("Starting server on {}...", bind_address);

	server->Wait();
	return 0;
}