#include "EngineTspSolutionController.hpp"
#include "TspAlgorithm.hpp"

#include <utility>

namespace assfire::tsp {

    EngineTspSolutionController::EngineTspSolutionController(const TspTask &task, AlgorithmPtr algorithm) : task(task), algorithm(std::move(algorithm)) {}

    EngineTspSolutionController::EngineTspSolutionController(const TspTask &task, AlgorithmPtr algorithm, TspAlgorithmStateContainer state_container)
            : task(task), algorithm(std::move(algorithm)), state_container(std::move(state_container)) {}

    void EngineTspSolutionController::start() {
        if (is_started) return;
        is_started = true;
        launchTask();
    }

    bool EngineTspSolutionController::isInterrupted() const {
        return is_interrupted;
    }

    bool EngineTspSolutionController::isPaused() const {
        return is_paused;
    }

    void EngineTspSolutionController::interrupt() {
        if (!is_started) return;
        is_interrupted = true;
    }

    void EngineTspSolutionController::pause() {
        if (!is_started || is_interrupted) return;
        is_paused = true;
    }

    void EngineTspSolutionController::resume() {
        if (!is_started || !is_paused || is_interrupted) return;
        is_paused = false;
        launchTask();
    }

    std::optional<TspSolution> EngineTspSolutionController::getCurrentSolution() {
        std::lock_guard<std::mutex> guard(solution_guard);
        return solution;
    }

    bool EngineTspSolutionController::isFinished() {
        return is_finished;
    }

    TspAlgorithmStateContainer &EngineTspSolutionController::getStateContainer() {
        return state_container;
    }

    void EngineTspSolutionController::publishSolution(const TspSolution &solution) {
        {
            std::lock_guard<std::mutex> guard(solution_guard);
            this->solution = solution;
        }
        if (solution.isFinalSolution()) is_finished = true;
        if (solution_listener) solution_listener(solution);
    }

    EngineTspSolutionController::~EngineTspSolutionController() {
        waitForTaskStop();
    }

    void EngineTspSolutionController::setSolutionListener(std::function<void(const TspSolution &)> listener) {
        this->solution_listener = listener;
    }

    void EngineTspSolutionController::launchTask() {
        waitForTaskStop();
        control_state = std::async(std::launch::async, [&, controller = this] {
            algorithm->solveTsp(task, *controller);
        });
    }

    void EngineTspSolutionController::waitForTaskStop() {
        if (control_state.valid()) control_state.wait();
    }

    const EngineTspSolutionController::SessionId &EngineTspSolutionController::getSessionId() const {
        return session_id;
    }
}