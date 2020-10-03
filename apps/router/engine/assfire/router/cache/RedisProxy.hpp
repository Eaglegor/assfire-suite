#pragma once

#include "assfire/router/RouteProvider.hpp"
#include "assfire/router/RoutingMetricsCollector.hpp"
#include <memory>

namespace cpp_redis
{
	class client;
}

namespace assfire::metrics
{
	class RedisProxy : public RouteProvider
	{
	public:
		RedisProxy(const std::string& host, std::size_t port, MetricsCollector metrics_context = MetricsCollector());
		~RedisProxy();

        GetSingleRouteResponse getRoute(GetSingleRouteRequest, long) const override;
        GetRoutesBatchResponse getRoutesBatch(GetRoutesBatchRequest, long) const override;
        void getRoutesBatch(GetRoutesBatchRequest, RoutesBatchConsumer, long) const override;

	private:
        GetRoutesBatchResponse calculateBatchUsingSingleRoutes(GetRoutesBatchRequest request, long request_id) const;
		std::unique_ptr<cpp_redis::client> redis_client;
		MetricsCollector metrics_collector;
	};
}