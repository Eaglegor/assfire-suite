#include "StdoutSolutionPublisher.hpp"
#include <assfire/tsp/api/TspSolution.hpp>
#include <assfire/tsp/api/TspTask.hpp>
#include <iostream>

namespace assfire::tsp::worker {

    void StdoutSolutionPublisher::publish(const std::string &task_id, const TspTask &task, const TspSolution &solution) {
        std::cout << "Got solution for task " << task_id << " with cost " << solution.getCost().getValue() << (solution.isFinalSolution() ? " (final) " : "") << std::endl;
        std::cout << "New sequence";
        for (const auto i : solution.getSequence()) {
            std::cout << " " << task.getPoints()[i].getId();
        }
        std::cout << std::endl;
    }

    void StdoutSolutionPublisher::onError(const std::string &task_id) {
        std::cout << "Got error for task " << task_id << std::endl;
    }
}