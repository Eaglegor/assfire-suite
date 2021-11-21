#pragma once

#include <assfire/api/v1/concepts/concepts.pb.h>
#include <assfire/concepts/Speed.hpp>

namespace assfire::api::v1::concepts {
    class SpeedTranslator {
    public:
        using ApiSpeed = assfire::Speed;
        using ProtoSpeed = assfire::api::v1::concepts::Speed;

        static ApiSpeed fromProto(const ProtoSpeed& obj);
        static ProtoSpeed toProto(const ApiSpeed& obj);
    };
}


