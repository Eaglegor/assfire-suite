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
		virtual ~RedisProxy();
		virtual routing::proto::RouteInfo getRoute(routing::proto::SingleRouteRequest, const Router&, long) const override;
		virtual void getRoutesBatch(routing::proto::ManyToManyRoutesRequest, std::function<void(routing::proto::RouteInfo)>, const Router&, long) const override;

	private:
		std::unique_ptr<cpp_redis::client> redis_client;
		RoutingMetricsCollector metrics_context;
	};
}