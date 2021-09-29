#pragma once

#include "assfire/api/v1/tsp/concepts.pb.h"
#include "assfire/tsp/api/TspEstimatorSettings.hpp"

namespace assfire::api::v1::tsp {
    class TspEstimatorSettingsTranslator {
    public:
        using ProtoEstimatorSettings = assfire::api::v1::tsp::TspEstimatorSettings;
        using ApiEstimatorSettings = assfire::tsp::TspEstimatorSettings;

        static ApiEstimatorSettings fromProto(const ProtoEstimatorSettings &value);
        static ProtoEstimatorSettings toProto(const ApiEstimatorSettings &value);
    };
}

