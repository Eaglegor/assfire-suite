#pragma once

#include "assfire/engine/router/RouteProvider.hpp"
#include "assfire/engine/router/MetricsCollector.hpp"
#include <memory>

namespace cpp_redis
{
	class client;
}

namespace assfire::router
{
	class RedisRouteProviderProxy : public RouteProvider
	{
	public:
		RedisRouteProviderProxy(std::unique_ptr<RouteProvider>&& backend, const std::string& host, std::size_t port, const MetricsCollector& metrics_collector = MetricsCollector());
		~RedisRouteProviderProxy() override;

        GetSingleRouteResponse getRoute(GetSingleRouteRequest, long) const override;
        GetRoutesBatchResponse getRoutesBatch(GetRoutesBatchRequest, long) const override;
        void getRoutesBatch(GetRoutesBatchRequest, RoutesBatchConsumer, long) const override;

	private:
        std::unique_ptr<RouteProvider> backend;
        std::unique_ptr<cpp_redis::client> redis_client;
        MetricsCollector metrics_collector;
	};
}