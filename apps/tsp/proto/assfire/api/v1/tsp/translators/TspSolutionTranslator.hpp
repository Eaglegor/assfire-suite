#pragma once

#include "assfire/api/v1/tsp/concepts.pb.h"
#include "assfire/tsp/api/TspSolution.hpp"

namespace assfire::api::v1::tsp {
    class TspSolutionTranslator {
    public:
        using ProtoTspSolution = assfire::api::v1::tsp::TspSolution;
        using ApiTspSolution = assfire::tsp::TspSolution;

        static ApiTspSolution fromProto(const ProtoTspSolution &value);
        static ProtoTspSolution toProto(const ApiTspSolution &value);
    };
}

