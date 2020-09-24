#include "GrpcClient.hpp"
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

using namespace assfire;

GrpcClient::GrpcClient(const std::string &host, int port, const RouteRequestSettings &settings, bool use_ssl) {
    channel = grpc::CreateChannel(host + ": " + std::to_string(port), createChannelCredentials(use_ssl));
    stub = routing::proto::v1::RoutesProvider::NewStub(channel);
    routing_options.CopyFrom(convertSettings(settings));
}

GrpcClient::GrpcClient(const std::string &host, int port, routing::proto::v1::RoutingOptions routing_options, bool use_ssl)
        : routing_options(routing_options) {
    channel = grpc::CreateChannel(host + ": " + std::to_string(port), createChannelCredentials(use_ssl));
    stub = routing::proto::v1::RoutesProvider::NewStub(channel);
}

RouteInfo GrpcClient::getSingleRoute(const Location &origin, const Location &destination) const {
    routing::proto::v1::GetSingleRouteRequest request;
    request.mutable_from()->set_latitude(origin.getLatitude());
    request.mutable_from()->set_longitude(origin.getLongitude());
    request.mutable_to()->set_latitude(destination.getLatitude());
    request.mutable_to()->set_longitude(destination.getLongitude());
    request.mutable_options()->CopyFrom(routing_options);

    routing::proto::v1::RouteInfo reply = getSingleRoute(request);

    return RouteInfo(reply.distance(), reply.duration());
}

routing::proto::v1::RouteInfo GrpcClient::getSingleRoute(const routing::proto::v1::GetSingleRouteRequest &request) const {
    grpc::ClientContext context;
    routing::proto::v1::GetSingleRouteResponse response;
    grpc::Status status = stub->GetSingleRoute(&context, request, &response);

    if (!status.ok()) {
        throw std::logic_error(status.error_message());
    }

    return response.route_info();
}

namespace { // See https://en.cppreference.com/w/cpp/utility/variant/visit

    // helper type for the visitor #4
    template<class... Ts>
    struct overloaded : Ts ... {
        using Ts::operator()...;
    };
    // explicit deduction guide (not needed as of C++20)
    template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;
}

routing::proto::v1::RoutingOptions GrpcClient::convertSettings(const RouteRequestSettings &settings) const {
    routing::proto::v1::RoutingOptions result;

    std::visit(overloaded{
            [&result](const EuclideanSettings &s) {
                result.set_routing_type(routing::proto::v1::RoutingOptions::EUCLIDEAN);
                result.set_velocity(s.velocity);
            },
            [&result](const RandomSettings &s) {
                result.set_routing_type(routing::proto::v1::RoutingOptions::RANDOM);
                result.set_velocity(s.velocity);
            }
    }, settings.calculation_settings);

    std::visit(overloaded{
            [&result](const FixedPointIntCoordinatesFormat &f) {
                result.mutable_coordinates_format()->set_precision(f.precision);
                result.mutable_coordinates_format()->set_type(routing::proto::v1::CoordinateFormat::FIXED_POINT_INT);
            }
    }, settings.coordinates_format);

    return result;
}

std::shared_ptr<grpc::ChannelCredentials> GrpcClient::createChannelCredentials(bool use_ssl) const {
    if (use_ssl) {
        return grpc::SslCredentials(grpc::SslCredentialsOptions());
    } else {
        return grpc::InsecureChannelCredentials();
    }
}
