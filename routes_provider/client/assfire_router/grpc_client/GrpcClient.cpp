#include "GrpcClient.hpp"
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

using namespace assfire;

GrpcClient::GrpcClient(const std::string& host, int port, const RouteRequestSettings& settings)
{
	channel = grpc::CreateChannel(host + ": " + std::to_string(port), grpc::InsecureChannelCredentials());
	stub = routing::proto::RoutesProvider::NewStub(channel);
	routing_options.CopyFrom(convertSettings(settings));
}

GrpcClient::GrpcClient(const std::string& host, int port, routing::proto::RoutingOptions routing_options):
	routing_options(routing_options) 
{
	channel = grpc::CreateChannel(host + ": " + std::to_string(port), grpc::InsecureChannelCredentials());
	stub = routing::proto::RoutesProvider::NewStub(channel);
}

RouteInfo GrpcClient::getSingleRoute(const Location& origin, const Location& destination) const
{
	routing::proto::SingleRouteRequest request;
	request.mutable_from()->set_latitude(origin.getLatitude());
	request.mutable_from()->set_longitude(origin.getLongitude());
	request.mutable_to()->set_latitude(destination.getLatitude());
	request.mutable_to()->set_longitude(destination.getLongitude());
	request.mutable_options()->CopyFrom(routing_options);

	routing::proto::RouteInfo reply = getSingleRoute(request);

	return RouteInfo(reply.distance(), reply.duration());
}

routing::proto::RouteInfo GrpcClient::getSingleRoute(const routing::proto::SingleRouteRequest& request) const
{
	grpc::ClientContext context;
	routing::proto::RouteInfo result;
	grpc::Status status = stub->GetSingleRoute(&context, request, &result);

	return result;
}

namespace { // See https://en.cppreference.com/w/cpp/utility/variant/visit

	// helper type for the visitor #4
	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	// explicit deduction guide (not needed as of C++20)
	template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;
}

routing::proto::RoutingOptions GrpcClient::convertSettings(const RouteRequestSettings& settings) const
{
	routing::proto::RoutingOptions result;

	std::visit(overloaded{
		[&result](const EuclideanSettings& s) {
			result.mutable_euclidean_routing_options()->set_velocity(s.velocity);
		},
		[&result](const RandomSettings& s) {
			result.mutable_random_routing_options();
		}
	}, settings.calculation_settings);

	std::visit(overloaded{
		[&result](const FixedPointIntCoordinatesFormat& f) {
			result.mutable_coordinates_format()->mutable_fixed_point_int()->set_precision(f.precision);
		}
		}, settings.coordinates_format);

	return result;
}
