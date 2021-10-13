#pragma once

#include "assfire/api/v1/tsp/worker.pb.h"

namespace assfire::tsp::worker {
    class SolutionPublisher {
    public:
        using Solution = assfire::api::v1::tsp::TspSolution;

        virtual void publishSolution(const Solution& solution) = 0;
    };
}