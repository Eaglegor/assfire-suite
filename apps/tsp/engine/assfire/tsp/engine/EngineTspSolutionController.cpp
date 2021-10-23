#include "EngineTspSolutionController.hpp"
#include "TspAlgorithm.hpp"
#include <spdlog/spdlog.h>
#include <utility>

namespace assfire::tsp {

    EngineTspSolutionController::EngineTspSolutionController(const SessionId &session_id, const TspTask &task, AlgorithmPtr algorithm, TspAlgorithmStateContainer state_container,
                                                             TspSolutionListener solution_listener)
            : session_id(session_id), task(task), algorithm(std::move(algorithm)), state_container(std::move(state_container)), solution_listener(solution_listener) {}

    void EngineTspSolutionController::start() {
        SPDLOG_DEBUG("Starting solution for session {}", session_id);
        resume();
    }

    bool EngineTspSolutionController::isInterrupted() const {
        return is_interrupted;
    }

    void EngineTspSolutionController::interrupt() {
        SPDLOG_DEBUG("Interrupting solution for session {}", session_id);
        is_interrupted = true;
    }

    void EngineTspSolutionController::pause() {
        SPDLOG_DEBUG("Pausing solution for session {}", session_id);
        is_interrupted = true;
    }

    void EngineTspSolutionController::resume() {
        SPDLOG_DEBUG("Resuming solution for session {}", session_id);
        launchTask();
    }

    bool EngineTspSolutionController::isFinished() {
        return is_finished;
    }

    TspAlgorithmStateContainer &EngineTspSolutionController::getStateContainer() {
        return state_container;
    }

    void EngineTspSolutionController::publishSolution(const TspSolution &solution) {
        SPDLOG_DEBUG("New solution found for session {}", session_id);
        is_finished = solution.isFinalSolution();
        solution_listener(solution);
    }

    EngineTspSolutionController::~EngineTspSolutionController() {
        is_interrupted = true;
        waitForTaskStop();
    }

    void EngineTspSolutionController::launchTask() {
        if (is_finished) {
            SPDLOG_DEBUG("Couldn't start tsp task for session {} - session already finished", session_id);
            return;
        }
        SPDLOG_DEBUG("Going to start tsp task for session {}", session_id);
        bool expected_started = false;
        if (is_started.compare_exchange_strong(expected_started, true)) {
            SPDLOG_DEBUG("Launching tsp task for session {}", session_id);
            waitForTaskStop();
            is_interrupted = false;
            control_state = std::async(std::launch::async, [&, controller = this] {
                algorithm->solveTsp(task, *controller);
                if (!is_finished) {
                    state_container.persist();
                }
                is_started = false;
            });
        } else {
            SPDLOG_DEBUG("Couldn't start tsp task for session {} - session already running", session_id);
        }
    }

    void EngineTspSolutionController::waitForTaskStop() {
        if (is_interrupted && control_state.valid()) {
            SPDLOG_DEBUG("Waiting until algorithm exits for session {}", session_id);
            control_state.wait();
        }
    }

    const EngineTspSolutionController::SessionId &EngineTspSolutionController::getSessionId() const {
        return session_id;
    }

    void EngineTspSolutionController::waitFor(long milliseconds) {
        if (control_state.valid()) {
            control_state.wait_for(std::chrono::milliseconds(milliseconds));
        }
    }
}