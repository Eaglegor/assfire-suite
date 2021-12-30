#include "TimeWastingTspAlgorithm.hpp"
#include <spdlog/spdlog.h>
#include <chrono>
#include <utility>
#include <fmt/ostream.h>
#include <random>
#include "assfire/tsp/api/io/Streams.hpp"
#include <thread>

using namespace std::literals::chrono_literals;

namespace assfire::tsp {

    namespace {
        std::string formatSequence(const TspTask &task, const TspSolution::Sequence &sequence) {
            std::string result;
            for (int i = 0; i < sequence.size(); ++i) {
                if (i > 0) result += "-";
                result += std::to_string(task.getPoints()[sequence[i]].getId());
            }
            return result;
        }
    }

    TimeWastingTspAlgorithm::TimeWastingTspAlgorithm(TspEstimator estimator) : estimator(std::move(estimator)) {}

    void TimeWastingTspAlgorithm::solveTsp(const TspTask &task, EngineTspSolutionController &solution_controller) const {
        SPDLOG_INFO("Starting time-wasting algorithm for tsp session {}", solution_controller.getSessionId());

        validateOrResetSavedState(task, solution_controller.getStateContainer(), solution_controller.getSessionId());

        TspSolution current_solution = createInitialSolution(task, solution_controller.getStateContainer(), solution_controller.getSessionId());

        solution_controller.publishSolution(current_solution);

        TspSolution::Sequence sequence = current_solution.getSequence();

        std::optional<State> current_iteration_state = createInitialState(task, solution_controller.getStateContainer(), solution_controller.getSessionId());

        while (current_iteration_state && !solution_controller.isInterrupted()) {
            int counter = current_iteration_state->getCounter();
            SPDLOG_DEBUG("Starting time-wasting iteration {} for session {}", counter, solution_controller.getSessionId());

            std::this_thread::sleep_for(1s);

            TspValidationResult validation_result = estimator.validate(sequence);
            if (validation_result.isValid()) {
                    int random_cost = std::rand() % 100000;
                    SPDLOG_TRACE("Solution {} accepted, new cost = {}", formatSequence(task, sequence), TspCost(random_cost));
                    current_solution = TspSolution(sequence, TspCost(random_cost), validation_result, false);
                    solution_controller.publishSolution(current_solution);
            }

            current_iteration_state = nextState(current_iteration_state);

            if (current_iteration_state && solution_controller.isInterrupted()) {
                SPDLOG_INFO("Interrupt signal received for tsp session {}. Saving state", solution_controller.getSessionId());
                saveState(*current_iteration_state, solution_controller.getStateContainer(), current_solution.getSequence(), task);
            }
        }

        solution_controller.publishSolution(current_iteration_state ? current_solution : current_solution.asFinal());
        SPDLOG_INFO("Finished time-wasting algorithm for tsp session {}", solution_controller.getSessionId());
    }

    TspSolution TimeWastingTspAlgorithm::createInitialSolution(const TspTask &task, const TspAlgorithmStateContainer &saved_state, const TspSolutionController::SessionId &session_id) const {
        TspSolution::Sequence initial_sequence;
        if (saved_state.containsTimeWastingAlgorithmState()) {
            SPDLOG_INFO("Loading saved sequence for session {}", session_id);
            initial_sequence = saved_state.getSequence();
        } else {
            for (int i = 0; i < task.getPoints().size(); ++i) {
                initial_sequence.push_back(i);
            }
        }
        SPDLOG_INFO("Initial sequence for session {}: {}", session_id, formatSequence(task, initial_sequence));
        return TspSolution(initial_sequence,
                           estimator.calculateCost(initial_sequence),
                           estimator.validate(initial_sequence),
                           false);
    }

    std::optional<TimeWastingTspAlgorithm::State>
    TimeWastingTspAlgorithm::createInitialState(const TspTask &task, const TspAlgorithmStateContainer &state_container, const TspSolutionController::SessionId &session_id) const {
        std::optional<State> loaded_state = loadState(task, state_container);
        if (loaded_state) {
            SPDLOG_INFO("Found algorithm saved state for session {}: counter={}", session_id, loaded_state->getCounter());
            return loaded_state;
        }

        return std::make_optional<State>(0);
    }

    std::optional<TimeWastingTspAlgorithm::State> TimeWastingTspAlgorithm::nextState(const std::optional<State> &state) const {
        if (!state) return state;
        if (state->getCounter() >= 99) return std::nullopt;
        return std::make_optional<State>(state->getCounter() + 1);
    }

    void TimeWastingTspAlgorithm::validateOrResetSavedState(const TspTask &task, TspAlgorithmStateContainer &saved_state, const TspSolutionController::SessionId &session_id) const {
        if (!saved_state.containsTimeWastingAlgorithmState()) {
            SPDLOG_INFO("No saved time-wasting algorithm state found for session {}. Resetting state container", session_id);
            saved_state.reset();
        } else if (!saved_state.canBeUsedFor(task)) {
            SPDLOG_INFO("Found saved state for session {} but it's inconsistent or incompatible with the task. Resetting state container", session_id);
            saved_state.reset();
        } else {
            const TspAlgorithmStateContainer::TimeWastingAlgorithmStateDto &dto_state = saved_state.getDto().time_wasting_algorithm_state();
            int counter = dto_state.counter();

            if (counter >= 99) {
                SPDLOG_WARN("Inconsistent saved time-wasting state found for session {}: counter = {}. Resetting state container", session_id, counter);
                saved_state.reset();
            }
        }
    }

    void
    TimeWastingTspAlgorithm::saveState(const TimeWastingTspAlgorithm::State &state, TspAlgorithmStateContainer &container, const TspSolution::Sequence &current_sequence, const TspTask &task) const {
        container.reset();
        container.setPoints(task.getPoints());
        container.setSequence(current_sequence);
        TspAlgorithmStateContainer::TimeWastingAlgorithmStateDto *dto_state = container.getDto().mutable_time_wasting_algorithm_state();
        dto_state->set_counter(state.getCounter());
    }

    std::optional<TimeWastingTspAlgorithm::State> TimeWastingTspAlgorithm::loadState(const TspTask &task, const TspAlgorithmStateContainer &container) const {
        if (container.containsTimeWastingAlgorithmState()) {
            const TspAlgorithmStateContainer::TimeWastingAlgorithmStateDto &dto_state = container.getDto().time_wasting_algorithm_state();
            int counter = dto_state.counter();
            return std::make_optional<State>(counter);
        }
        return std::nullopt;
    }

    TimeWastingTspAlgorithm::State::State(int counter) : counter(counter) {

    }

    int TimeWastingTspAlgorithm::State::getCounter() const {
        return counter;
    }
}