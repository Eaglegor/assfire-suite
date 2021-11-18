#pragma once

#include <string>
#include "assfire/tsp/worker/SolutionPublisher.hpp"

namespace assfire::tsp {
    class StdoutSolutionPublisher : public SolutionPublisher {
    public:
        void publish(const std::string &task_id, const TspTask& task, const TspSolution &solution) override;

        void onError(const std::string &task_id) override;
    };
}