#pragma once

#include "assfire/api/v1/tsp/concepts.pb.h"
#include "assfire/tsp/api/TspSolverSettings.hpp"

namespace assfire::api::v1::tsp {
    class TspSolverSettingsTranslator {
    public:
        using ProtoSolverSettings = assfire::api::v1::tsp::TspSolverSettings;

        using ApiSolverSettings = assfire::tsp::TspSolverSettings;

        static ApiSolverSettings fromProto(const ProtoSolverSettings &value);
        static ProtoSolverSettings toProto(const ApiSolverSettings &value);
    };
}

