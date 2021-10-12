#pragma once

#include "assfire/api/v1/tsp/concepts.pb.h"
#include "assfire/tsp/api/TspAlgorithmSettings.hpp"

namespace assfire::api::v1::tsp {
    class TspAlgorithmSettingsTranslator {
    public:
        using ProtoAlgorithmSettings = assfire::api::v1::tsp::TspAlgorithmSettings;

        using ApiAlgorithmSettings = assfire::tsp::TspAlgorithmSettings;

        static ApiAlgorithmSettings fromProto(const ProtoAlgorithmSettings &value);
        static ProtoAlgorithmSettings toProto(const ApiAlgorithmSettings &value);
    };
}

