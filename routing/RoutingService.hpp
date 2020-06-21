#pragma once
#include <routing.grpc.pb.h>
#include <backends/euclidean/EuclideanRouter.hpp>
#include <tuple>

namespace assfire
{
	class RoutingService final : public routing::RoutesProvider::Service
	{
	public:
		grpc::Status GetSingleRoute(grpc::ServerContext*, const routing::SingleRouteRequest*, routing::RouteInfo*) override;
		grpc::Status GetRoutesBatch(grpc::ServerContext*, const routing::ManyToManyRoutesRequest*, grpc::ServerWriter<routing::RouteInfo>*) override;
		grpc::Status GetStreamingRoutesBatch(grpc::ServerContext*, grpc::ServerReaderWriter<routing::RouteInfo, routing::ManyToManyRoutesRequest>*) override;

	private:
		const Router& selectRouter(const routing::RoutingOptions& options) const;

		std::tuple<EuclideanRouter> routers;
	};
}