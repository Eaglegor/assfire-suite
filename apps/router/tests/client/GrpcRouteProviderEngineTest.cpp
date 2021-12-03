#include <gtest/gtest.h>
#include <assfire/router/client/GrpcRouteProviderEngine.hpp>
#include <assfire/api/v1/router/translators/RoutingProfileTranslator.hpp>
#include "assfire/api/v1/locations/translators/LocationTranslator.hpp"
#include <assfire/api/v1/router/translators/RouteProviderSettingsTranslator.hpp>
#include <assfire/api/v1/router/translators/RouteInfoTranslator.hpp>
#include "MockProtobufClient.hpp"

using namespace assfire;
using namespace assfire::router;
using namespace assfire::locations;

using RouterEngineTypeTranslator = assfire::api::v1::router::RouterEngineTypeTranslator;
using RoutingProfileTranslator = assfire::api::v1::router::RoutingProfileTranslator;
using RouteProviderSettingsTranslator = assfire::api::v1::router::RouteProviderSettingsTranslator;
using LocationTranslator = assfire::api::v1::locations::LocationTranslator;
using RouteInfoTranslator = assfire::api::v1::router::RouteInfoTranslator;


class GrpcRouteProviderEngineTest : public ::testing::Test {
public:
    Location getLocation(double lat, double lon) const {
        return Location::fromDoubleLatLon(lat, lon);
    }

    Location getLocation(double coord) const {
        return getLocation(coord, coord);
    }

    std::vector<Location> buildLocations(const std::vector<int> coord) {
        std::vector<Location> result;
        for (int c : coord) {
            result.push_back(getLocation(c));
        }
        return result;
    }

    ProtobufClient::GetSingleRouteRequest
    buildSingleRouteRequest(const Location &from, const Location &to, RouterEngineType engine_type, const RouteProviderSettings &settings, const RoutingProfile &routing_profile) {
        ProtobufClient::GetSingleRouteRequest request;
        request.mutable_origin()->CopyFrom(LocationTranslator::toProto(from));
        request.mutable_destination()->CopyFrom(LocationTranslator::toProto(to));
        request.mutable_settings()->CopyFrom(RouteProviderSettingsTranslator::toProto(settings, engine_type));
        request.mutable_routing_profile()->CopyFrom(RoutingProfileTranslator::toProto(routing_profile));
        return request;
    }

    ProtobufClient::GetSingleRouteResponse buildSingleRouteResponse(const Location &from, const Location &to, double meters, long seconds) {
        RouteDetails details(RouteInfo(Distance::fromMeters(meters), TimeInterval::fromSeconds(seconds)), {});
        ProtobufClient::GetSingleRouteResponse response;
        response.mutable_status()->set_code(assfire::api::v1::router::ResponseStatus_Code_RESPONSE_STATUS_CODE_OK);
        response.mutable_route_info()->CopyFrom(RouteInfoTranslator::toProto(from, to, details));
        return response;
    }

    ProtobufClient::GetSingleRouteResponse buildSingleRouteResponse(const Location &from, const Location &to, double meters, long seconds, std::vector<int> waypoints) {
        RouteDetails details(Distance::fromMeters(meters), TimeInterval::fromSeconds(seconds), buildLocations(waypoints));
        ProtobufClient::GetSingleRouteResponse response;
        response.mutable_status()->set_code(assfire::api::v1::router::ResponseStatus_Code_RESPONSE_STATUS_CODE_OK);
        response.mutable_route_info()->CopyFrom(RouteInfoTranslator::toProto(from, to, details));
        return response;
    }

    ProtobufClient::GetRoutesBatchRequest buildRoutesBatchRequest(const std::vector<int> &from_loc,
                                                                  const std::vector<int> &to_loc,
                                                                  RouterEngineType engine_type,
                                                                  const RouteProviderSettings &settings,
                                                                  const RoutingProfile &routing_profile) {
        ProtobufClient::GetRoutesBatchRequest request;

        for (int coord : from_loc) {
            request.add_origins()->CopyFrom(LocationTranslator::toProto(getLocation(coord, coord)));
        }

        for (int coord : to_loc) {
            request.add_destinations()->CopyFrom(LocationTranslator::toProto(getLocation(coord, coord)));
        }

        request.mutable_settings()->CopyFrom(RouteProviderSettingsTranslator::toProto(settings, engine_type));
        request.mutable_routing_profile()->CopyFrom(RoutingProfileTranslator::toProto(routing_profile));
        return request;
    }

    ProtobufClient::GetRoutesBatchResponse buildRoutesBatchResponse(const std::vector<int> &from_loc,
                                                                    const std::vector<int> &to_loc,
                                                                    const std::vector<std::vector<int>> &seconds) {

        ProtobufClient::GetRoutesBatchResponse response;
        response.mutable_status()->set_code(assfire::api::v1::router::ResponseStatus_Code_RESPONSE_STATUS_CODE_OK);

        for (int i = 0; i < from_loc.size(); ++i) {
            for (int j = 0; j < to_loc.size(); ++j) {
                auto *ri = response.add_route_infos();
                ri->CopyFrom(RouteInfoTranslator::toProto(getLocation(from_loc[i], from_loc[i]), getLocation(to_loc[j], to_loc[j]),
                                                          RouteInfo(Distance::fromMeters(seconds[i][j]), TimeInterval::fromSeconds(seconds[i][j]))));
            }
        }

        return response;
    }

    ProtobufClient::GetRoutesBatchResponse buildRoutesBatchResponse(const std::vector<int> &from_loc,
                                                                    const std::vector<int> &to_loc,
                                                                    const std::vector<std::vector<int>> &seconds,
                                                                    const std::vector<std::vector<std::vector<int>>> &waypoints) {

        ProtobufClient::GetRoutesBatchResponse response;
        response.mutable_status()->set_code(assfire::api::v1::router::ResponseStatus_Code_RESPONSE_STATUS_CODE_OK);

        for (int i = 0; i < from_loc.size(); ++i) {
            for (int j = 0; j < to_loc.size(); ++j) {
                auto *ri = response.add_route_infos();
                ri->CopyFrom(RouteInfoTranslator::toProto(getLocation(from_loc[i], from_loc[i]), getLocation(to_loc[j], to_loc[j]),
                                                          RouteDetails(Distance::fromMeters(seconds[i][j]), TimeInterval::fromSeconds(seconds[i][j]), buildLocations(waypoints[i][j]))));
            }
        }

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

    protobuf_client.addResponse(buildSingleRouteRequest(from, to, assfire::router::RouterEngineType::CROWFLIGHT, settings, routing_profile),
                                buildSingleRouteResponse(from, to, 50, 50));

    RouteInfo response = engine.getSingleRouteInfo(from, to);

    ASSERT_EQ(response.getDuration().toSeconds(), 50);
    ASSERT_DOUBLE_EQ(response.getDistance().toMeters(), 50);
}

TEST_F(GrpcRouteProviderEngineTest, RequestsSingleRouteDetails) {
    MockProtobufClient protobuf_client;
    Location from = getLocation(10, 10);
    Location to = getLocation(20, 20);
    RouteProviderSettings settings;
    RoutingProfile routing_profile(Speed::fromKilometersPerHour(60));

    GrpcRouteProviderEngine engine(protobuf_client, assfire::router::RouterEngineType::CROWFLIGHT, settings, routing_profile);

    protobuf_client.addResponse(buildSingleRouteRequest(from, to, assfire::router::RouterEngineType::CROWFLIGHT, settings, routing_profile),
                                buildSingleRouteResponse(from, to, 50, 50, {1,2,3}));

    RouteDetails response = engine.getSingleRouteDetails(from, to);

    ASSERT_EQ(response.getDuration().toSeconds(), 50);
    ASSERT_DOUBLE_EQ(response.getDistance().toMeters(), 50);
    ASSERT_EQ(response.getWaypoints().size(), 3);
    ASSERT_EQ(response.getWaypoints()[0], getLocation(1));
    ASSERT_EQ(response.getWaypoints()[1], getLocation(2));
    ASSERT_EQ(response.getWaypoints()[2], getLocation(3));

}

TEST_F(GrpcRouteProviderEngineTest, RequestsRouteInfoMatrix) {
    MockProtobufClient protobuf_client;
    RouteProviderSettings settings;
    RoutingProfile routing_profile(Speed::fromKilometersPerHour(60));

    std::vector<int> from
            {1, 2, 3};

    std::vector<int> to
            {4, 2, 5};

    std::vector<std::vector<int>> seconds
            {
                    {100, 10,  20}, // 100s must be ignored as they represent self-self route (0 must be returned by engine)
                    {30,  100, 40},
                    {50,  60,  100}
            };


    GrpcRouteProviderEngine engine(protobuf_client, assfire::router::RouterEngineType::CROWFLIGHT, settings, routing_profile);

    protobuf_client.addResponse(buildRoutesBatchRequest(from, to, assfire::router::RouterEngineType::CROWFLIGHT, settings, routing_profile),
                                buildRoutesBatchResponse(from, to, seconds));

    auto result = engine.getRouteInfoMatrix(buildLocations(from), buildLocations(to));

    ASSERT_EQ(result.getRowsCount(), 3);
    ASSERT_EQ(result.getColumnsCount(), 3);

    for (int c = 0; c < result.getColumnsCount(); ++c) {
        for (int r = 0; r < result.getRowsCount(); ++r) {
            if (c == r) {
                EXPECT_TRUE(result.at(r, c).isZero());
            } else {
                EXPECT_EQ(result.at(r, c).getDuration().toSeconds(), seconds[r][c]);
                EXPECT_DOUBLE_EQ(result.at(r, c).getDistance().toMeters(), seconds[r][c]);
            }
        }
    }
}

TEST_F(GrpcRouteProviderEngineTest, RequestsRouteDetailsMatrix) {
    MockProtobufClient protobuf_client;
    RouteProviderSettings settings;
    RoutingProfile routing_profile(Speed::fromKilometersPerHour(60));

    std::vector<int> from
            {1, 2, 3};

    std::vector<int> to
            {4, 2, 5};

    std::vector<std::vector<int>> seconds
            {
                    {100, 10,  20}, // 100s must be ignored as they represent self-self route (0 must be returned by engine)
                    {30,  100, 40},
                    {50,  60,  100}
            };

    std::vector<std::vector<std::vector<int>>> waypoints
            { // Diagonals must be ignored as well as 100s for seconds
                    {{1,  2,  3},  {4,  5, 6}, {7,  8, 9}},
                    {{10, 11},     {12, 13},   {14, 15}},
                    {{16, 17, 18}, {19, 20},   {21, 22}}
            };

    GrpcRouteProviderEngine engine(protobuf_client, assfire::router::RouterEngineType::CROWFLIGHT, settings, routing_profile);

    protobuf_client.addResponse(buildRoutesBatchRequest(from, to, assfire::router::RouterEngineType::CROWFLIGHT, settings, routing_profile),
                                buildRoutesBatchResponse(from, to, seconds, waypoints));

    auto result = engine.getRouteDetailsMatrix(buildLocations(from), buildLocations(to));

    ASSERT_EQ(result.getRowsCount(), 3);
    ASSERT_EQ(result.getColumnsCount(), 3);

    for (int c = 0; c < result.getColumnsCount(); ++c) {
        for (int r = 0; r < result.getRowsCount(); ++r) {
            Location origin = getLocation(from[r], from[r]);
            Location destination = getLocation(to[c], to[c]);
            if (c == r) {
                EXPECT_TRUE(result.at(r, c).isZero());
                EXPECT_EQ(result.at(r, c).getWaypoints().size(), 2);
                EXPECT_EQ(result.at(r, c).getWaypoints()[0], origin);
                EXPECT_EQ(result.at(r, c).getWaypoints()[1], destination);
            } else {
                EXPECT_EQ(result.at(r, c).getDuration().toSeconds(), seconds[r][c]);
                EXPECT_DOUBLE_EQ(result.at(r, c).getDistance().toMeters(), seconds[r][c]);

                const auto &wps = waypoints[r][c];
                EXPECT_EQ(result.at(r, c).getWaypoints().size(), wps.size());
                for (int i = 0; i < wps.size(); ++i) {
                    EXPECT_EQ(result.at(r, c).getWaypoints()[i], getLocation(wps[i], wps[i]));
                }
            }
        }
    }
}