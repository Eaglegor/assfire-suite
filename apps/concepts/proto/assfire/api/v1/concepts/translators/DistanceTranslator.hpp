#pragma once

#include <assfire/api/v1/concepts/concepts.pb.h>
#include "assfire/concepts/Distance.hpp"

namespace assfire::api::v1::concepts {
    class DistanceTranslator {
    public:
        using ApiDistance = assfire::Distance;
        using ProtoDistance = assfire::api::v1::concepts::Distance;

        static ApiDistance fromProto(const ProtoDistance& rhs);
        static ProtoDistance toProto(const ApiDistance& rhs);
    };
}


