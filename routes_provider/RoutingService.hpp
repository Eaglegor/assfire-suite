#pragma once
#include <routing.grpc.pb.h>
#include <backends/euclidean/EuclideanRouter.hpp>
#include <backends/random/RandomRouter.hpp>
#include <backends/redis/RouterProxy.hpp>
#include "RoutingMetricsCollector.hpp"
#include <memory>
#include <atomic>

namespace assfire
{
	class RoutingService final : public routing::proto::RoutesProvider::Service
	{
	public:
		RoutingService(bool, const std::string&, std::size_t, RoutingMetricsCollector);

		grpc::Status GetSingleRoute(grpc::ServerContext*, const routing::proto::SingleRouteRequest*, routing::proto::RouteInfo*) override;
		grpc::Status GetRoutesBatch(grpc::ServerContext*, const routing::proto::ManyToManyRoutesRequest*, grpc::ServerWriter<routing::proto::RouteInfo>*) override;
		grpc::Status GetStreamingRoutesBatch(grpc::ServerContext*, grpc::ServerReaderWriter<routing::proto::RouteInfo, routing::proto::ManyToManyRoutesRequest>*) override;

	private:
		const Router& selectRouter(const routing::proto::RoutingOptions&, long) const;

		RoutingMetricsCollector metrics_context;
		std::unique_ptr<RouterProxy> caching_proxy;
		std::atomic_long request_id_counter;

		EuclideanRouter euclidean_router;
		RandomRouter random_router;
	};
}