#pragma once

#include <assfire/api/v1/router/concepts.pb.h>
#include <assfire/router/api/model/RoutingProfile.hpp>

namespace assfire::api::v1::router {
    class RoutingProfileTranslator {
    public:
        using ProtoRoutingProfile = assfire::api::v1::router::RoutingProfile;
        using ProtoSpeed = assfire::api::v1::concepts::Speed;

        using ApiRoutingProfile = assfire::router::RoutingProfile;
        using ApiSpeed = assfire::Speed;

        static ApiRoutingProfile fromProto(const ProtoRoutingProfile& profile);

        static ProtoRoutingProfile toProto(const ApiRoutingProfile& profile);

    private:
        static ApiSpeed fromProtoSpeed(const ProtoSpeed& speed);

        static ProtoSpeed toProtoSpeed(const ApiSpeed& speed);
    };
}

