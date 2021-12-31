#include "TwoOptTspAlgorithm.hpp"
#include "assfire/tsp/api/TspTask.hpp"
#include <optional>
#include <algorithm>
#include <spdlog/spdlog.h>

namespace assfire::tsp {
    TwoOptTspAlgorithm::TwoOptTspAlgorithm(TspEstimator estimator) : estimator(std::move(estimator)) {}

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

    void TwoOptTspAlgorithm::validateOrResetSavedState(const TspTask &task, TspAlgorithmStateContainer &saved_state, const EngineTspSolutionController::SessionId &session_id) const {
        if (!saved_state.containsTwoOptAlgorithmState()) {
            SPDLOG_INFO("No saved 2-opt algorithm state found for session {}. Resetting state container", session_id);
            saved_state.reset();
        } else if (!saved_state.canBeUsedFor(task)) {
            SPDLOG_INFO("Found saved state for session {} but it's inconsistent or incompatible with the task. Resetting state container", session_id);
            saved_state.reset();
        } else {
            const TspAlgorithmStateContainer::TwoOptAlgorithmStateDto &two_opt_state = saved_state.getDto().two_opt_algorithm_state();
            int i = two_opt_state.i();
            int j = two_opt_state.j();
            int k = two_opt_state.k();
            int size = task.getPoints().size();

            if (i >= size || j >= size || k >= size) {
                SPDLOG_WARN("Inconsistent saved two-opt state found for session {}: i = {}, j = {}, k = {} while size = {}. Resetting state container", session_id, i, j, k, size);
                saved_state.reset();
            }
        }
    }

    TspSolution TwoOptTspAlgorithm::createInitialSolution(const TspTask &task, const TspAlgorithmStateContainer &saved_state, const EngineTspSolutionController::SessionId &session_id) const {
        TspSolution::Sequence initial_sequence;
        if (saved_state.containsTwoOptAlgorithmState()) {
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

    void TwoOptTspAlgorithm::solveTsp(const TspTask &task, EngineTspSolutionController &solution_controller) const {
        SPDLOG_INFO("Starting 2-opt algorithm for tsp session {}", solution_controller.getSessionId());

        validateOrResetSavedState(task, solution_controller.getStateContainer(), solution_controller.getSessionId());

        TspSolution current_solution = createInitialSolution(task, solution_controller.getStateContainer(), solution_controller.getSessionId());

        solution_controller.publishSolution(current_solution);

        TspSolution::Sequence sequence = current_solution.getSequence();

        std::optional<State> current_iteration_state = createInitialState(task, solution_controller.getStateContainer(), solution_controller.getSessionId());

        while (current_iteration_state && !solution_controller.isInterrupted()) {
            int i = current_iteration_state->getI();
            int j = current_iteration_state->getJ();

            std::reverse(sequence.begin() + i, sequence.begin() + j);
            TspValidationResult validation_result = estimator.validate(sequence);
            if (validation_result.isValid()) {
                TspCost cost = estimator.calculateCost(sequence);
                if (cost < current_solution.getCost()) {
                    SPDLOG_DEBUG("Solution {} accepted, new cost = {}", formatSequence(task, sequence), cost.getValue());
                    current_solution = TspSolution(sequence, cost, validation_result, false);
                    solution_controller.publishSolution(current_solution);
                } else {
                    SPDLOG_TRACE("Solution {} rejected: cost {} >= {} (best)", formatSequence(task, sequence), cost.getValue(), current_solution.getCost().getValue());
                    std::reverse(sequence.begin() + i, sequence.begin() + j);
                }
            } else {
                std::reverse(sequence.begin() + i, sequence.begin() + j);
            }

            current_iteration_state = nextState(current_iteration_state);

            if (current_iteration_state && solution_controller.isInterrupted()) {
                SPDLOG_INFO("Interrupt signal received for tsp session {}. Saving state", solution_controller.getSessionId());
                saveState(*current_iteration_state, solution_controller.getStateContainer(), current_solution.getSequence(), task);
            }
        }

        solution_controller.publishSolution(current_iteration_state ? current_solution : current_solution.asFinal());
        SPDLOG_INFO("Finished 2-opt algorithm for tsp session {}", solution_controller.getSessionId());
    }

    std::optional<TwoOptTspAlgorithm::State>
    TwoOptTspAlgorithm::createInitialState(const TspTask &task, const TspAlgorithmStateContainer &state_container, const EngineTspSolutionController::SessionId &session_id) const {

        std::optional<State> loaded_state = loadState(task, state_container);
        if (loaded_state) {
            SPDLOG_INFO("Found algorithm saved state for session {}: i={}, j={}, k={}", session_id, loaded_state->getI(), loaded_state->getJ(), loaded_state->getK());
            return loaded_state;
        }

        return std::make_optional<State>(1, 2, 0, task.getPoints().size());
    }

    std::optional<TwoOptTspAlgorithm::State> TwoOptTspAlgorithm::nextState(const std::optional<State> &state) const {
        if (!state) return state;
        int i = state->getI();
        int j = state->getJ();
        int k = state->getK();
        int size = state->getSize();

        if (++j >= size) {
            if (++i >= size - 1) {
                ++k;
                i = 1;
                j = 2;
            } else {
                j = i + 1;
            }
        }

        SPDLOG_TRACE("New 2-opt state: {}, {}, {}", i, j, k);

        return k >= size ? std::nullopt : std::make_optional<State>(i, j, k, size);
    }

    void TwoOptTspAlgorithm::saveState(const TwoOptTspAlgorithm::State &state, TspAlgorithmStateContainer &container, const TspSolution::Sequence &current_sequence, const TspTask &task) const {
        container.reset();
        container.setPoints(task.getPoints());
        container.setSequence(current_sequence);
        TspAlgorithmStateContainer::TwoOptAlgorithmStateDto *two_opt_state = container.getDto().mutable_two_opt_algorithm_state();
        two_opt_state->set_i(state.getI());
        two_opt_state->set_j(state.getJ());
        two_opt_state->set_k(state.getK());
    }

    std::optional<TwoOptTspAlgorithm::State> TwoOptTspAlgorithm::loadState(const TspTask &task, const TspAlgorithmStateContainer &container) const {
        if (container.containsTwoOptAlgorithmState()) {
            const TspAlgorithmStateContainer::TwoOptAlgorithmStateDto &two_opt_state = container.getDto().two_opt_algorithm_state();
            int i = two_opt_state.i();
            int j = two_opt_state.j();
            int k = two_opt_state.k();
            int size = task.getPoints().size();
            return std::make_optional<State>(i, j, k, size);
        }
        return std::nullopt;
    }

    int TwoOptTspAlgorithm::State::getI() const {
        return i;
    }

    int TwoOptTspAlgorithm::State::getJ() const {
        return j;
    }

    int TwoOptTspAlgorithm::State::getK() const {
        return k;
    }

    int TwoOptTspAlgorithm::State::getSize() const {
        return size;
    }

    TwoOptTspAlgorithm::State::State(int i, int j, int k, int size) : i(i), j(j), k(k), size(size) {}
}