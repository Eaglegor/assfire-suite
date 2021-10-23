#pragma once

#include <string>
#include "SolutionPublisher.hpp"

namespace assfire::tsp::worker {
    class StdoutSolutionPublisher : public SolutionPublisher {
    public:
        void publish(const std::string &task_id, const TspSolution &solution) override;
    };
}