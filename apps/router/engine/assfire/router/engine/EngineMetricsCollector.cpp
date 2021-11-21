#include "EngineMetricsCollector.hpp"

using namespace assfire::router;

EngineMetricsCollector::Stopwatch EngineMetricsCollector::measureSingleCrowflightRouteInfoCalculation()
{
    return Stopwatch([&](long ns) {
        if(metrics_consumer) metrics_consumer->recordSingleCrowflightRouteInfoCalculationTime(ns);
    });
}

void EngineMetricsCollector::recordSingleCrowflightRouteInfoCalculation()
{
    if(metrics_consumer) metrics_consumer->recordSingleCrowflightRouteInfoCalculation();
}

void EngineMetricsCollector::recordSingleCrowflightRouteDetailsCalculation()
{
    if(metrics_consumer) metrics_consumer->recordSingleCrowflightRouteDetailsCalculation();
}

EngineMetricsCollector::Stopwatch EngineMetricsCollector::measureSingleEuclideanRouteInfoCalculation()
{
    return Stopwatch([&](long ns) {
        if(metrics_consumer) metrics_consumer->recordSingleEuclideanRouteInfoCalculationTime(ns);
    });
}

void EngineMetricsCollector::recordSingleEuclideanRouteInfoCalculation()
{
    if(metrics_consumer) metrics_consumer->recordSingleEuclideanRouteInfoCalculation();
}

void EngineMetricsCollector::recordSingleEuclideanRouteDetailsCalculation()
{
    if(metrics_consumer) metrics_consumer->recordSingleEuclideanRouteDetailsCalculation();
}

EngineMetricsCollector::Stopwatch EngineMetricsCollector::measureSingleRandomRouteInfoCalculation()
{
    return Stopwatch([&](long ns) {
        if(metrics_consumer) metrics_consumer->recordSingleRandomRouteInfoCalculationTime(ns);
    });
}

void EngineMetricsCollector::recordSingleRandomRouteInfoCalculation()
{
    if(metrics_consumer) metrics_consumer->recordSingleRandomRouteInfoCalculation();
}

void EngineMetricsCollector::recordSingleRandomRouteDetailsCalculation()
{
    if(metrics_consumer) metrics_consumer->recordSingleRandomRouteDetailsCalculation();
}

EngineMetricsCollector::Stopwatch EngineMetricsCollector::measureSingleOsrmRouteDetailsCalculation(OsrmGeometry geometry)
{
    return Stopwatch([&](long ns) {
        if(metrics_consumer) metrics_consumer->recordSingleOsrmRouteDetailsCalculationTime(ns, geometry);
    });
}

void EngineMetricsCollector::recordSingleOsrmRouteInfoCalculation(OsrmGeometry geometry)
{
    if(metrics_consumer) metrics_consumer->recordSingleOsrmRouteInfoCalculation(geometry);
}

void EngineMetricsCollector::recordSingleOsrmRouteDetailsCalculation(OsrmGeometry geometry)
{
    if(metrics_consumer) metrics_consumer->recordSingleOsrmRouteDetailsCalculation(geometry);
}

EngineMetricsCollector::Stopwatch EngineMetricsCollector::measureSingleRouteDetailsRetrievalFromRedis()
{
    return Stopwatch([&](long ns) {
        if(metrics_consumer) metrics_consumer->recordSingleRouteDetailsRetrievalFromRedisTime(ns);
    });
}

void EngineMetricsCollector::recordSingleRouteInfoRetrievalFromRedis()
{
    if(metrics_consumer) metrics_consumer->recordSingleRouteInfoRetrievalFromRedis();
}

void EngineMetricsCollector::recordSingleRouteDetailsRetrievalFromRedis()
{
    if(metrics_consumer) metrics_consumer->recordSingleRouteDetailsRetrievalFromRedis();
}

void EngineMetricsCollector::recordRedisCacheHit()
{
    if(metrics_consumer) metrics_consumer->recordRedisCacheHit();
}

void EngineMetricsCollector::recordRedisCacheMiss()
{
    if(metrics_consumer) metrics_consumer->recordRedisCacheMiss();
}
