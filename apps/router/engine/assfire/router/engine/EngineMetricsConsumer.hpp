#pragma once

#include "assfire/router/api/RouteProviderSettings.hpp"

namespace assfire::router {
    class EngineMetricsConsumer {
    public:
        virtual ~EngineMetricsConsumer() = default;

        virtual void recordSingleCrowflightRouteInfoCalculationTime(long nanoseconds) = 0;

        virtual void recordSingleCrowflightRouteInfoCalculation() = 0;
        virtual void recordSingleCrowflightRouteDetailsCalculation() = 0;

        virtual void recordSingleEuclideanRouteInfoCalculationTime(long nanoseconds) = 0;
        virtual void recordSingleEuclideanRouteInfoCalculation() = 0;
        virtual void recordSingleEuclideanRouteDetailsCalculation() = 0;

        virtual void recordSingleRandomRouteInfoCalculationTime(long nanoseconds) = 0;
        virtual void recordSingleRandomRouteInfoCalculation() = 0;
        virtual void recordSingleRandomRouteDetailsCalculation() = 0;

        virtual void recordSingleOsrmRouteDetailsCalculationTime(long nanoseconds, OsrmGeometry geometry) = 0;
        virtual void recordSingleOsrmRouteInfoCalculation(OsrmGeometry geometry) = 0;
        virtual void recordSingleOsrmRouteDetailsCalculation(OsrmGeometry geometry) = 0;

        virtual void recordSingleRouteDetailsRetrievalFromRedisTime(long nanoseconds) = 0;
        virtual void recordSingleRouteInfoRetrievalFromRedis() = 0;
        virtual void recordSingleRouteDetailsRetrievalFromRedis() = 0;
        virtual void recordRedisCacheHit() = 0;
        virtual void recordRedisCacheMiss() = 0;
    };
}