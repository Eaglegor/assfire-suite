#pragma once

#include <routing.grpc.pb.h>
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
		GrpcClient(const std::string& host, int port, routing::proto::RoutingOptions options, bool use_ssl);

		RouteInfo getSingleRoute(const Location& origin, const Location& destination) const;

		routing::proto::RouteInfo getSingleRoute(const routing::proto::SingleRouteRequest& request) const;

	private:
		routing::proto::RoutingOptions convertSettings(const RouteRequestSettings& settings) const;
		std::shared_ptr<grpc::ChannelCredentials> createChannelCredentials(bool use_ssl) const;

		std::shared_ptr<grpc::ChannelInterface> channel;
		std::unique_ptr<routing::proto::RoutesProvider::Stub> stub;
		routing::proto::RoutingOptions routing_options;
	};
}