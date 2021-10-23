#include "StdoutSolutionPublisher.hpp"
#include <assfire/tsp/api/TspSolution.hpp>
#include <iostream>

namespace assfire::tsp::worker {

    void StdoutSolutionPublisher::publish(const std::string &task_id, const TspSolution &solution) {
        std::cout << "Got solution for task " << task_id << " with cost " << solution.getCost().getValue() << std::endl;
    }
}