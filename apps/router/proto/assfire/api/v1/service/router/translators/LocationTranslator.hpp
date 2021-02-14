#pragma once

#include <assfire/api/v1/service/router/router.grpc.pb.h>
#include <assfire/api/router/model/Location.hpp>

namespace assfire::router {
    class LocationTranslator {
    public:
        static Location fromProto(const assfire::api::v1::model::routing::Location &location) {
            return Location(Coordinate((long)location.lat()), Coordinate((long)location.lon()));
        }

        static assfire::api::v1::model::routing::Location toProto(const Location &location) {
            assfire::api::v1::model::routing::Location result;
            result.set_lat(location.getLatitude().longValue());
            result.set_lon(location.getLongitude().longValue());
            return result;
        }
    };
}