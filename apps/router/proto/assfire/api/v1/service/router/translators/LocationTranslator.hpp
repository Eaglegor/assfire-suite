#pragma once

#include <assfire/api/v1/service/router/router.grpc.pb.h>
#include "../../../../../../../../../libs/route_optimization_concepts/assfire/concepts/Location.hpp"

namespace assfire::router::proto_translation {
    class LocationTranslator {
    public:
        static Location fromProto(const assfire::api::v1::model::routing::Location &location) {
            return Location(Coordinate::fromEncodedValue(location.lat()), Coordinate::fromEncodedValue(location.lon()));
        }

        static assfire::api::v1::model::routing::Location toProto(const Location &location) {
            assfire::api::v1::model::routing::Location result;
            result.set_lat(location.getLatitude().encodedValue());
            result.set_lon(location.getLongitude().encodedValue());
            return result;
        }
    };
}