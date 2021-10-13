#pragma once

#include "SolutionPublisher.hpp"

namespace assfire::tsp::worker {
    class RabbitMQSolutionPublisher : public SolutionPublisher {
    public:
        void publishSolution(const Solution &solution) override;
    };
}