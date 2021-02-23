#pragma once

#include <assfire/api/v1/service/router/router.grpc.pb.h>
#include <assfire/router/api/model/RoutingProfile.hpp>

namespace assfire::router::proto_translation {

    class RoutingProfileTranslator {
    public:
        static RoutingProfile fromProto(const assfire::api::v1::model::routing::RoutingProfile& profile) {
            return RoutingProfile(fromProtoVelocity(profile.velocity()));
        }

        static assfire::api::v1::model::routing::RoutingProfile toProto(const RoutingProfile& profile) {
            assfire::api::v1::model::routing::RoutingProfile result;
            result.set_velocity(toProtoVelocity(profile.getSpeed()));
            return result;
        }

    private:
        static Speed fromProtoVelocity(double velocity) {
            return Speed::fromMetersPerSecond(velocity);
        }

        static double toProtoVelocity(const Speed& speed) {
            return speed.toMetersPerSecond();
        }
    };

}

