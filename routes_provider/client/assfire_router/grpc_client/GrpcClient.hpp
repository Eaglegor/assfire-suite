#pragma once

#include <assfire/routing/v1/routing.grpc.pb.h>
#include <memory>
#include "../Location.hpp"
#include "../RouteInfo.hpp"
#include <grpcpp/channel.h>
#include <grpcpp/security/credentials.h>
#include "../settings/RouteRequestSettings.hpp"

namespace assfire
{
	class GrpcClient {
	public:
		GrpcClient(const std::string& host, int port, const RouteRequestSettings& settings, bool use_ssl);
		GrpcClient(const std::string& host, int port, routing::proto::v1::RoutingOptions options, bool use_ssl);

		RouteInfo getSingleRoute(const Location& origin, const Location& destination) const;

		routing::proto::v1::RouteInfo getSingleRoute(const routing::proto::v1::GetSingleRouteRequest& request) const;

	private:
		routing::proto::v1::RoutingOptions convertSettings(const RouteRequestSettings& settings) const;
		std::shared_ptr<grpc::ChannelCredentials> createChannelCredentials(bool use_ssl) const;

		std::shared_ptr<grpc::ChannelInterface> channel;
		std::unique_ptr<routing::proto::v1::RoutesProvider::Stub> stub;
		routing::proto::v1::RoutingOptions routing_options;
	};
}