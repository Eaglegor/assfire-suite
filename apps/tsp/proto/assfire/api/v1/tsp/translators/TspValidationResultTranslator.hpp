#pragma once

#include "assfire/api/v1/tsp/concepts.pb.h"
#include "assfire/tsp/api/TspValidationResult.hpp"

namespace assfire::api::v1::tsp {
    class TspValidationResultTranslator {
    public:
        using ProtoValidationResult = assfire::api::v1::tsp::TspValidationResult;
        using ApiValidationResult = assfire::tsp::TspValidationResult;

        static ApiValidationResult fromProto(const ProtoValidationResult &value);
        static ProtoValidationResult toProto(const ApiValidationResult &value);
    };
}

