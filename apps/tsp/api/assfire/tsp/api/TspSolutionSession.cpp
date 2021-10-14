#pragma once

#include "TspSolutionSession.hpp"

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

    std::future<TspSolution> TspSolutionSession::getCurrentSolution() {
        return solution_controller->getCurrentSolution();
    }

    bool TspSolutionSession::isFinished() {
        return solution_controller->isFinished();
    }
}