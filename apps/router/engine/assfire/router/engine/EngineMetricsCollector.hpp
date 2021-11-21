#pragma once

#include <functional>
#include <memory>
#include <chrono>
#include <assfire/metrics/stopwatch.hpp>
#include <assfire/router/api/RouterEngineType.hpp>
#include "EngineMetricsConsumer.hpp"
#include "assfire/router/api/RouteProviderSettings.hpp"

namespace assfire::router {
	class EngineMetricsCollector
	{
	public:
	    using Stopwatch = assfire::metrics::stopwatch;

        Stopwatch measureSingleCrowflightRouteInfoCalculation();
        void recordSingleCrowflightRouteInfoCalculation();
        void recordSingleCrowflightRouteDetailsCalculation();

        Stopwatch measureSingleEuclideanRouteInfoCalculation();
        void recordSingleEuclideanRouteInfoCalculation();
        void recordSingleEuclideanRouteDetailsCalculation();

        Stopwatch measureSingleRandomRouteInfoCalculation();
        void recordSingleRandomRouteInfoCalculation();
        void recordSingleRandomRouteDetailsCalculation();

        Stopwatch measureSingleOsrmRouteDetailsCalculation(OsrmGeometry geometry);
        void recordSingleOsrmRouteInfoCalculation(OsrmGeometry geometry);
        void recordSingleOsrmRouteDetailsCalculation(OsrmGeometry geometry);

        Stopwatch measureSingleRouteDetailsRetrievalFromRedis();
        void recordSingleRouteInfoRetrievalFromRedis();
        void recordSingleRouteDetailsRetrievalFromRedis();
        void recordRedisCacheHit();
        void recordRedisCacheMiss();

	private:
	   std::shared_ptr<EngineMetricsConsumer> metrics_consumer;
    };
}