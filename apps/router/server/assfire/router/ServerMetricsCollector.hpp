#pragma once

#include <functional>
#include <memory>
#include <chrono>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include <assfire/engine/router/MetricsCollector.hpp>

namespace assfire::router {
	class ServerMetricsCollector : public MetricsCollector
	{
	public:
	    using Stopwatch = assfire::metrics::stopwatch;

		enum class RequestMode {
			SINGLE,
			BATCH,
			STREAMING_BATCH
		};

        ServerMetricsCollector();
        ServerMetricsCollector(std::shared_ptr<prometheus::Exposer> exposer);
        ServerMetricsCollector(const ServerMetricsCollector& rhs) = default;

		void addRequestedRoutes(int count, RequestMode mode) const;
		Stopwatch routesRequestStopwatch(int count, RequestMode mode) const;
		
		void addEuclideanCalculatedRoutes(int count, RequestMode mode) const;
		void addRandomCalculatedRoutes(int count, RequestMode mode) const;
        void addCrowflightCalculatedRoutes(int count, RequestMode mode) const;

		Stopwatch euclideanRoutesStopwatch(int count, RequestMode mode) const;
        Stopwatch randomRoutesStopwatch(int count, RequestMode mode) const;
        Stopwatch crowflightRoutesStopwatch(int count, RequestMode mode) const;

		void addRoutesCacheRequests(int count, RequestMode mode) const;
		void addRoutesCacheHits(int count) const;
		void addRoutesCacheMisses(int count) const;
		Stopwatch routesCacheStopwatch(int count, RequestMode mode) const;

	private:
		std::shared_ptr<prometheus::Exposer> exposer;
		std::shared_ptr<prometheus::Registry> registry;

		prometheus::Family<prometheus::Counter>* requested_routes_count_family;
		prometheus::Counter* requested_routes_count_single_mode;
		prometheus::Counter* requested_routes_count_batch_mode;
		prometheus::Counter* requested_routes_count_streaming_batch_mode;

		prometheus::Family<prometheus::Summary>* routes_request_processing_time_family;
		prometheus::Summary* routes_request_processing_time_single_mode;
		prometheus::Summary* routes_request_processing_time_batch_mode;
		prometheus::Summary* routes_request_processing_time_streaming_batch_mode;

		prometheus::Family<prometheus::Counter>* calculated_routes_count_family;
		prometheus::Counter* calculated_routes_count_euclidean;
		prometheus::Counter* calculated_routes_count_random;
        prometheus::Counter* calculated_routes_count_crowflight;

		prometheus::Family<prometheus::Summary>* routes_calculation_time_family;
		prometheus::Summary* routes_calculation_time_euclidean_single_mode;
		prometheus::Summary* routes_calculation_time_euclidean_batch_mode;
        prometheus::Summary* routes_calculation_time_euclidean_streaming_batch_mode;
        prometheus::Summary* routes_calculation_time_random_single_mode;
        prometheus::Summary* routes_calculation_time_random_batch_mode;
        prometheus::Summary* routes_calculation_time_random_streaming_batch_mode;
        prometheus::Summary* routes_calculation_time_crowflight_single_mode;
        prometheus::Summary* routes_calculation_time_crowflight_batch_mode;
        prometheus::Summary* routes_calculation_time_crowflight_streaming_batch_mode;
		
		prometheus::Family<prometheus::Counter>* routes_cache_requests_count_family;
		prometheus::Counter* routes_cache_requests_count;

		prometheus::Family<prometheus::Counter>* routes_cache_hits_family;
		prometheus::Counter* routes_cache_hits_count;
		prometheus::Counter* routes_cache_misses_count;

		prometheus::Family<prometheus::Summary>* routes_cache_retrieval_time_family;
		prometheus::Summary* routes_cache_retrieval_time;
	};
}