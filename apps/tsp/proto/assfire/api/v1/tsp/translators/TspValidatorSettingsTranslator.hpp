#pragma once

#include "assfire/api/v1/tsp/concepts.pb.h"
#include "assfire/tsp/api/TspValidatorSettings.hpp"

namespace assfire::api::v1::tsp {
    class TspValidatorSettingsTranslator {
    public:
        using ProtoValidatorSettings = assfire::api::v1::tsp::TspValidatorSettings;
        using ProtoValidatorType = assfire::api::v1::tsp::TspValidatorType;

        using ApiValidatorSettings = assfire::tsp::TspValidatorSettings;
        using ApiValidatorType = assfire::tsp::TspValidatorType;

        static ApiValidatorSettings fromProto(const ProtoValidatorSettings &value);
        static ProtoValidatorSettings toProto(const ApiValidatorSettings &value);

        static ApiValidatorType fromProto(const ProtoValidatorType &value);
        static ProtoValidatorType toProto(const ApiValidatorType &value);
    };
}

