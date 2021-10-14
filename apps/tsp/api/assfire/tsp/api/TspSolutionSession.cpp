#pragma once

#include "TspSolutionSession.hpp"

#include <utility>

namespace assfire::tsp {
    TspSolutionSession::TspSolutionSession(std::unique_ptr<TspSolutionController> solution_controller)
            : solution_controller(std::move(solution_controller)) {}

    void TspSolutionSession::interrupt() {
        solution_controller->interrupt();
    }

    void TspSolutionSession::pause() {
        solution_controller->pause();
    }

    void TspSolutionSession::resume() {
        solution_controller->resume();
    }

    std::optional<TspSolution> TspSolutionSession::getCurrentSolution() {
        return solution_controller->getCurrentSolution();
    }

    void TspSolutionSession::setSolutionListener(std::function<void(const TspSolution &)> solution_listener) {
        solution_controller->setSolutionListener(std::move(solution_listener));
    }

    bool TspSolutionSession::isFinished() {
        return solution_controller->isFinished();
    }
}