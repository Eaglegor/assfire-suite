#pragma once

#include <assfire/api/v1/locations/concepts.pb.h>
#include "assfire/locations/api/Location.hpp"

namespace assfire::api::v1::locations {
    class LocationTranslator {
    public:
        using ProtoLocation = assfire::api::v1::locations::Location;
        using ApiLocation = assfire::locations::Location;

        static ApiLocation fromProto(const ProtoLocation &location);

        static ProtoLocation toProto(const ApiLocation &location);
    };
}