#pragma once

#include <assfire/api/v1/router/concepts.pb.h>
#include "assfire/concepts/Location.hpp"

namespace assfire::api::v1::router {
    class LocationTranslator {
    public:
        using ProtoLocation = assfire::api::v1::concepts::Location;
        using ApiLocation = assfire::Location;

        static ApiLocation fromProto(const ProtoLocation &location);

        static ProtoLocation toProto(const ApiLocation &location);
    };
}