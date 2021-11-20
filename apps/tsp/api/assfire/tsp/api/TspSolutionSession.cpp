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

    bool TspSolutionSession::isFinished() {
        return solution_controller->isFinished();
    }

    void TspSolutionSession::waitFor(long milliseconds) {
        solution_controller->waitFor(milliseconds);
    }

    const TspSolutionSession::Id &TspSolutionSession::getId() const {
        return solution_controller->getSessionId();
    }
}