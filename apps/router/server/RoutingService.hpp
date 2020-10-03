#pragma once
#include <assfire/routing/v1/routing.grpc.pb.h>
#include "backends/euclidean/EuclideanRouter.hpp"
#include "backends/random/RandomRouter.hpp"
#include "backends/crowflight/CrowflightRouter.hpp"
#include "backends/redis/RouterProxy.hpp"
#include "backends/osrm/HttpOsrmRouter.hpp"
#include "assfire/router/MetricsCollector.hpp"
#include <memory>
#include <atomic>

namespace assfire
{
class RoutingService final : public assfire::routing::proto::v1::RoutesProvider::Service
	{
	public:
		RoutingService(bool, const std::string&, std::size_t, const RoutingMetricsCollector&, const std::string&);

		grpc::Status GetSingleRoute(grpc::ServerContext*, const routing::proto::v1::GetSingleRouteRequest*, routing::proto::v1::GetSingleRouteResponse*) override;
		grpc::Status GetRoutesBatch(grpc::ServerContext*, const routing::proto::v1::GetRoutesBatchRequest*, grpc::ServerWriter<routing::proto::v1::GetRoutesBatchResponse>*) override;
		grpc::Status GetStreamingRoutesBatch(grpc::ServerContext*, grpc::ServerReaderWriter<routing::proto::v1::GetRoutesBatchResponse, routing::proto::v1::GetRoutesBatchRequest>*) override;

	private:
		const Router& selectRouter(const routing::proto::v1::RoutingOptions&, long) const;

		RoutingMetricsCollector metrics_context;
		std::unique_ptr<RouterProxy> caching_proxy;
		std::atomic_long request_id_counter;

		EuclideanRouter euclidean_router;
		RandomRouter random_router;
		CrowflightRouter crowflight_router;
		HttpOsrmRouter osrm_router;
	};
}