#pragma once

#include "Router.hpp"
#include "RouterProxy.hpp"
#include "RoutingMetricsCollector.hpp"
#include <memory>

namespace cpp_redis
{
	class client;
}

namespace assfire 
{
	class RedisProxy : public RouterProxy
	{
	public:
		RedisProxy(const std::string& host, std::size_t port, RoutingMetricsCollector metrics_context = RoutingMetricsCollector());
		~RedisProxy();

        routing::proto::v1::GetSingleRouteResponse getRoute(routing::proto::v1::GetSingleRouteRequest, const Router& backend, long) const override;
        routing::proto::v1::GetRoutesBatchResponse getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest, const Router& backend, long) const override;
        void getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest, Router::RoutesBatchConsumer, const Router& backend, long) const override;

	private:
        routing::proto::v1::GetRoutesBatchResponse calculateBatchUsingSingleRoutes(routing::proto::v1::GetRoutesBatchRequest request, const Router& backend, long request_id) const;

		std::unique_ptr<cpp_redis::client> redis_client;
		RoutingMetricsCollector metrics_context;
	};
}