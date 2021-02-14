#include "ServerMetricsCollector.hpp"
#include <prometheus/counter.h>
#include <prometheus/gauge.h>
#include <prometheus/histogram.h>
#include <prometheus/summary.h>
#include <spdlog/spdlog.h>

using namespace assfire::router;

ServerMetricsCollector::ServerMetricsCollector() = default;

ServerMetricsCollector::ServerMetricsCollector(std::shared_ptr<prometheus::Exposer> exposer):
	exposer(exposer),
	registry(std::make_shared<prometheus::Registry>())
{
	SPDLOG_INFO("Initializing metrics collector...");

	requested_routes_count_family = &prometheus::BuildCounter()
		.Name("assfire_router_requested_routes_count")
		.Help("Count of requested routes")
		.Register(*registry);
	requested_routes_count_single_mode = &requested_routes_count_family->Add({ {"mode", "single"} });
	requested_routes_count_batch_mode = &requested_routes_count_family->Add({ {"mode", "batch"} });
	requested_routes_count_streaming_batch_mode = &requested_routes_count_family->Add({ {"mode", "streaming_batch"} });

	using Quantiles = prometheus::Summary::Quantiles;
	Quantiles common_quantiles{ {0.99, 0.1}, {0.95, 0.1}, {0.9, 0.1}, {0.5, 0.1} };
	routes_request_processing_time_family = &prometheus::BuildSummary()
		.Name("assfire_router_routes_request_processing_time")
		.Help("Routes request processing time")
		.Register(*registry);
	routes_request_processing_time_single_mode = &routes_request_processing_time_family->Add({ {"mode", "single"} }, common_quantiles);
	routes_request_processing_time_batch_mode = &routes_request_processing_time_family->Add({ {"mode", "batch"} }, common_quantiles);
	routes_request_processing_time_streaming_batch_mode = &routes_request_processing_time_family->Add({ {"mode", "streaming_batch"} }, common_quantiles);

	calculated_routes_count_family = &prometheus::BuildCounter()
		.Name("assfire_router_calculated_routes_count")
		.Help("Count of routes calculated by the backend routers")
		.Register(*registry);
	calculated_routes_count_euclidean = &calculated_routes_count_family->Add({ {"backend", "euclidean"} });
	calculated_routes_count_random = &calculated_routes_count_family->Add({ {"backend", "random"} });
    calculated_routes_count_crowflight = &calculated_routes_count_family->Add({ {"backend", "crowflight"} });

	routes_calculation_time_family = &prometheus::BuildSummary()
		.Name("assfire_router_routes_calculation_time")
		.Help("Time spent by backend to calculate route")
		.Register(*registry);
	routes_calculation_time_euclidean_single_mode = &routes_calculation_time_family->Add({ {"backend", "euclidean"}, {"mode", "single"} }, common_quantiles);
	routes_calculation_time_euclidean_batch_mode = &routes_calculation_time_family->Add({ {"backend", "euclidean"}, {"mode", "batch"} }, common_quantiles);
    routes_calculation_time_euclidean_streaming_batch_mode = &routes_calculation_time_family->Add({ {"backend", "euclidean"}, {"mode", "streaming_batch"} }, common_quantiles);

    routes_calculation_time_random_single_mode = &routes_calculation_time_family->Add({ {"backend", "random"}, {"mode", "single"} }, common_quantiles);
    routes_calculation_time_random_batch_mode = &routes_calculation_time_family->Add({ {"backend", "random"}, {"mode", "batch"} }, common_quantiles);
    routes_calculation_time_random_streaming_batch_mode = &routes_calculation_time_family->Add({ {"backend", "random"}, {"mode", "streaming_batch"} }, common_quantiles);

    routes_calculation_time_crowflight_single_mode = &routes_calculation_time_family->Add({ {"backend", "crowflight"}, {"mode", "single"} }, common_quantiles);
    routes_calculation_time_crowflight_batch_mode = &routes_calculation_time_family->Add({ {"backend", "crowflight"}, {"mode", "batch"} }, common_quantiles);
    routes_calculation_time_crowflight_streaming_batch_mode = &routes_calculation_time_family->Add({ {"backend", "crowflight"}, {"mode", "streaming_batch"} }, common_quantiles);

	routes_cache_requests_count_family = &prometheus::BuildCounter()
		.Name("assfire_router_routes_cache_requests_count")
		.Help("L2 routes cache requests count")
		.Register(*registry);
		routes_cache_requests_count = &routes_cache_requests_count_family->Add({});

	routes_cache_hits_family = &prometheus::BuildCounter()
		.Name("assfire_router_routes_cache_hits_count")
		.Help("Routes cache hits/misses count")
		.Register(*registry);
		routes_cache_hits_count = &routes_cache_hits_family->Add({ {"type", "hit"} });
		routes_cache_misses_count = &routes_cache_hits_family->Add({ {"type", "miss"} });

	routes_cache_retrieval_time_family = &prometheus::BuildSummary()
		.Name("assfire_router_routes_cache_retrieval_time")
		.Help("Time spent to retrieve route from L2 cache")
		.Register(*registry);
	routes_cache_retrieval_time = &routes_cache_retrieval_time_family->Add({}, common_quantiles);

	SPDLOG_INFO("Registering metrics...");
	exposer->RegisterCollectable(registry);
}

MetricsCollector::Stopwatch ServerMetricsCollector::routesRequestStopwatch(int count, ServerMetricsCollector::RequestMode mode) const
{
	if(!exposer) return MetricsCollector::Stopwatch();
	
	return MetricsCollector::Stopwatch(
		[this,mode](long ns) {
			switch (mode)
			{
			case RequestMode::SINGLE:
				routes_request_processing_time_single_mode->Observe(ns);
				break;
			case RequestMode::BATCH:
				routes_request_processing_time_batch_mode->Observe(ns);
				break;
			case RequestMode::STREAMING_BATCH:
				routes_request_processing_time_streaming_batch_mode->Observe(ns);
				break;
			default:
				break;
			}
			SPDLOG_TRACE("Route requst processing time: {} ns", ns);
		});
}

void ServerMetricsCollector::addRequestedRoutes(int count, ServerMetricsCollector::RequestMode mode) const
{
	if (!exposer) return;

	switch (mode)
	{
	case RequestMode::SINGLE:
		requested_routes_count_single_mode->Increment(count);
		break;
	case RequestMode::BATCH:
		requested_routes_count_batch_mode->Increment(count);
		break;
	case RequestMode::STREAMING_BATCH:
		requested_routes_count_streaming_batch_mode->Increment(count);
		break;
	}
}

void ServerMetricsCollector::addEuclideanCalculatedRoutes(int count, RequestMode mode) const
{
	if (!exposer) return;

	calculated_routes_count_euclidean->Increment(count);
}

void ServerMetricsCollector::addRandomCalculatedRoutes(int count, RequestMode mode) const
{
	if (!exposer) return;

	calculated_routes_count_random->Increment(count);
}

void ServerMetricsCollector::addCrowflightCalculatedRoutes(int count, RequestMode mode) const
{
    if (!exposer) return;

    calculated_routes_count_random->Increment(count);
}

MetricsCollector::Stopwatch ServerMetricsCollector::euclideanRoutesStopwatch(int count, RequestMode mode) const
{
	if (!exposer) return MetricsCollector::Stopwatch();

	return MetricsCollector::Stopwatch([this, mode](long ns) {
		switch (mode)
		{
		case RequestMode::SINGLE:
			routes_calculation_time_euclidean_single_mode->Observe(ns);
			break;
		case RequestMode::BATCH:
            routes_calculation_time_euclidean_batch_mode->Observe(ns);
            break;
		case RequestMode::STREAMING_BATCH:
			routes_calculation_time_euclidean_streaming_batch_mode->Observe(ns);
			break;
		default:
			break;
		}
		SPDLOG_TRACE("Euclidean routes calculation time: {} ns", ns);
		});
}

MetricsCollector::Stopwatch ServerMetricsCollector::randomRoutesStopwatch(int count, RequestMode mode) const
{
    if (!exposer) return MetricsCollector::Stopwatch();

    return MetricsCollector::Stopwatch([this, mode](long ns) {
        switch (mode)
        {
            case RequestMode::SINGLE:
                routes_calculation_time_random_single_mode->Observe(ns);
                break;
            case RequestMode::BATCH:
                routes_calculation_time_random_batch_mode->Observe(ns);
                break;
            case RequestMode::STREAMING_BATCH:
                routes_calculation_time_random_streaming_batch_mode->Observe(ns);
                break;
            default:
                break;
        }
        SPDLOG_TRACE("Random routes calculation time: {} ns", ns);
    });
}

MetricsCollector::Stopwatch ServerMetricsCollector::crowflightRoutesStopwatch(int count, RequestMode mode) const
{
    if (!exposer) return MetricsCollector::Stopwatch();

    return MetricsCollector::Stopwatch([this, mode](long ns) {
        switch (mode)
        {
            case RequestMode::SINGLE:
                routes_calculation_time_crowflight_single_mode->Observe(ns);
                break;
            case RequestMode::BATCH:
                routes_calculation_time_crowflight_batch_mode->Observe(ns);
                break;
            case RequestMode::STREAMING_BATCH:
                routes_calculation_time_crowflight_streaming_batch_mode->Observe(ns);
                break;
            default:
                break;
        }
        SPDLOG_TRACE("Crowflight routes calculation time: {} ns", ns);
    });
}

void ServerMetricsCollector::addRoutesCacheRequests(int count, RequestMode mode) const
{
	if (!exposer) return;

	routes_cache_requests_count->Increment(count);
}

void ServerMetricsCollector::addRoutesCacheHits(int count) const
{
	if (!exposer) return;

	routes_cache_hits_count->Increment(count);
}

void ServerMetricsCollector::addRoutesCacheMisses(int count) const
{
	if (!exposer) return;

	routes_cache_misses_count->Increment(count);
}

MetricsCollector::Stopwatch ServerMetricsCollector::routesCacheStopwatch(int count, RequestMode mode) const
{
	if(!exposer) return MetricsCollector::Stopwatch();

	return MetricsCollector::Stopwatch([this, mode](long ns) {
			routes_cache_retrieval_time->Observe(ns);
			SPDLOG_TRACE("Cached route retrieval time: {} ns", ns);
	});
}
