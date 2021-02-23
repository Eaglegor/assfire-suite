#include <gtest/gtest.h>
#include <assfire/router/client/GrpcRouteProviderEngine.hpp>
#include <assfire/api/v1/service/router/translators/RoutingProfileTranslator.hpp>
#include <assfire/api/v1/service/router/translators/LocationTranslator.hpp>
#include <assfire/api/v1/service/router/translators/RoutingOptionsTranslator.hpp>
#include <assfire/api/v1/service/router/translators/RouteInfoTranslator.hpp>
#include "MockProtobufClient.hpp"

using namespace assfire::router;
using namespace assfire::router::proto_translation;

class GrpcRouteProviderEngineTest : public ::testing::Test {
public:
    Location getLocation(double lat, double lon) const {
        return Location::fromDoubleLatLon(lat, lon);
    }

    ProtobufClient::GetSingleRouteRequest buildRequest(const Location& from, const Location& to, RouterEngineType engine_type, const RouteProviderSettings& settings, const RoutingProfile& routing_profile){
        ProtobufClient::GetSingleRouteRequest request;
        request.mutable_origin()->CopyFrom(LocationTranslator::toProto(from));
        request.mutable_destination()->CopyFrom(LocationTranslator::toProto(to));
        request.mutable_options()->CopyFrom(RoutingOptionsTranslator::toProto(settings, engine_type));
        request.mutable_routing_profile()->CopyFrom(RoutingProfileTranslator::toProto(routing_profile));
        return request;
    }

    ProtobufClient::GetSingleRouteResponse buildResponse(const Location& from, const Location& to, double meters, long seconds){
        RouteDetails details(RouteInfo(Distance::fromMeters(meters), TimeInterval::fromSeconds(seconds)), {});
        ProtobufClient::GetSingleRouteResponse response;
        response.mutable_status()->set_code(assfire::api::v1::service::router::ResponseStatus_Code_RESPONSE_STATUS_CODE_OK);
        response.mutable_route_info()->mutable_origin()->CopyFrom(LocationTranslator::toProto(from));
        response.mutable_route_info()->mutable_destination()->CopyFrom(LocationTranslator::toProto(to));
        response.mutable_route_info()->set_distance(details.getDistance().toMeters());
        response.mutable_route_info()->set_duration(details.getDuration().toSeconds());
        return response;
    }
};

TEST_F(GrpcRouteProviderEngineTest, RequestsSingleRouteInfo) {
    MockProtobufClient protobuf_client;
    Location from = getLocation(10, 10);
    Location to = getLocation(20, 20);
    RouteProviderSettings settings;
    RoutingProfile routing_profile(Speed::fromKilometersPerHour(60));

    GrpcRouteProviderEngine engine(protobuf_client, assfire::router::RouterEngineType::CROWFLIGHT, settings, routing_profile);

    protobuf_client.addResponse(buildRequest(from, to, assfire::router::RouterEngineType::CROWFLIGHT, settings, routing_profile),
                                buildResponse(from, to, 50, 50));

    RouteInfo response = engine.getSingleRouteInfo(from, to);

    ASSERT_EQ(response.getDuration().toSeconds(), 50);
    ASSERT_DOUBLE_EQ(response.getDistance().toMeters(), 50);
}

