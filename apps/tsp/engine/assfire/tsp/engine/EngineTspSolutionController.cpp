#include "EngineTspSolutionController.hpp"
#include "TspAlgorithm.hpp"

#include <utility>

namespace assfire::tsp {

    EngineTspSolutionController::EngineTspSolutionController(AlgorithmPtr algorithm) : algorithm(std::move(algorithm)) {}

    EngineTspSolutionController::EngineTspSolutionController(AlgorithmPtr algorithm, TspAlgorithmStateContainer state_container)
            : algorithm(std::move(algorithm)), state_container(std::move(state_container)) {}

    void EngineTspSolutionController::start() {

    }

    bool EngineTspSolutionController::isInterrupted() const {
        return false;
    }

    bool EngineTspSolutionController::isPaused() const {
        return false;
    }

    void EngineTspSolutionController::interrupt() {

    }

    void EngineTspSolutionController::pause() {

    }

    void EngineTspSolutionController::resume() {

    }

    std::future<TspSolution> EngineTspSolutionController::getCurrentSolution() {
        return std::future<TspSolution>();
    }

    bool EngineTspSolutionController::isFinished() {
        return false;
    }

    TspAlgorithmStateContainer &EngineTspSolutionController::getStateContainer() {
        return state_container;
    }

    void EngineTspSolutionController::publishSolution(const TspSolution &solution) {

    }

}