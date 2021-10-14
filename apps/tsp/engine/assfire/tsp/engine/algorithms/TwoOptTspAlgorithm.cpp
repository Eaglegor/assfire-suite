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

    void TwoOptTspAlgorithm::validateOrResetSavedState(const TspTask &task, TspAlgorithmStateContainer &saved_state) const {
        if (!saved_state.containsTwoOptState()) {
            SPDLOG_INFO("No saved 2-opt algorithm state found. Resetting state container");
            saved_state.reset();
        } else if (!saved_state.isConsistent()) {
            SPDLOG_INFO("Found saved state but it's inconsistent. Resetting state container");
            saved_state.reset();
        } else if (!saved_state.isCompatibleWith(task)) {
            SPDLOG_INFO("Found saved state but it's incompatible with current task. Resetting state container");
            saved_state.reset();
        } else {
            const TspAlgorithmStateContainer::TwoOptAlgorithmStateDto &two_opt_state = saved_state.getDto().two_opt_algorithm_state();
            int i = two_opt_state.i();
            int j = two_opt_state.j();
            int k = two_opt_state.k();
            int size = task.getPoints().size();

            if (i >= size || j >= size || k >= size) {
                SPDLOG_WARN("Inconsistent saved two-opt state found: i = {}, j = {}, k = {} while size = {}. Resetting state container", i, j, k, size);
                saved_state.reset();
            }
        }
    }

    TspSolution TwoOptTspAlgorithm::createInitialSolution(const TspTask &task, const TspAlgorithmStateContainer &saved_state) const {
        TspSolution::Sequence initial_sequence;
        if (saved_state.containsTwoOptState()) {
            SPDLOG_INFO("Loading saved sequence");
            initial_sequence = saved_state.getSequence();
        } else {
            for (int i = 0; i < task.getPoints().size(); ++i) {
                initial_sequence.push_back(i);
            }
        }
        SPDLOG_INFO("Initial sequence: {}", formatSequence(task, initial_sequence));
        return TspSolution(initial_sequence,
                           estimator.calculateCost(task.getPoints(), initial_sequence),
                           estimator.validate(task.getPoints(), initial_sequence),
                           false);
    }

    void TwoOptTspAlgorithm::solveTsp(const TspTask &task, EngineTspSolutionController &solution_controller) const {
        validateOrResetSavedState(task, solution_controller.getStateContainer());

        TspSolution solution = createInitialSolution(task, solution_controller.getStateContainer());

        solution_controller.publishSolution(solution);

        TspSolution::Sequence sequence = solution.getSequence();

        std::optional<State> current_iteration_state = createInitialState(task, solution_controller.getStateContainer());

        while (current_iteration_state && !solution_controller.isInterrupted() && !solution_controller.isPaused()) {
            int i = current_iteration_state->getI();
            int j = current_iteration_state->getJ();

            std::reverse(sequence.begin() + i, sequence.begin() + j);
            TspValidationResult validation_result = estimator.validate(task.getPoints(), sequence);
            if (validation_result.isValid()) {
                TspCost cost = estimator.calculateCost(task.getPoints(), sequence);
                if (cost < solution.getCost()) {
                    solution_controller.publishSolution(TspSolution(sequence, cost, validation_result, false));
                } else {
                    std::reverse(sequence.begin() + i, sequence.begin() + j);
                }
            } else {
                std::reverse(sequence.begin() + i, sequence.begin() + j);
            }

            current_iteration_state = nextState(current_iteration_state);

            if (current_iteration_state && solution_controller.isPaused()) {
                saveState(*current_iteration_state, solution_controller.getStateContainer(), solution.getSequence(), task);
            }
        }

        solution_controller.publishSolution(current_iteration_state ? solution : solution.asFinal());
    }

    std::optional<TwoOptTspAlgorithm::State> TwoOptTspAlgorithm::createInitialState(const TspTask &task, const TspAlgorithmStateContainer &state_container) const {

        std::optional<State> loaded_state = loadState(task, state_container);
        if (loaded_state) {
            SPDLOG_INFO("Found algorithm saved state: i={}, j={}, k={}", loaded_state->getI(), loaded_state->getJ(), loaded_state->getK());
            return loaded_state;
        }

        return std::make_optional<State>(0, 1, 0, task.getPoints().size());
    }

    std::optional<TwoOptTspAlgorithm::State> TwoOptTspAlgorithm::nextState(const std::optional<State> &state) const {
        if (!state) return state;
        int i = state->getI();
        int j = state->getJ();
        int k = state->getK();
        int size = state->getSize();

        if (++j >= size) {
            if (++i >= size) {
                ++k;
                i = 0;
                j = 1;
            } else {
                j = i + 1;
            }
        }

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
        const TspAlgorithmStateContainer::TspAlgorithmStateDto &dto = container.getDto();
        if (dto.has_two_opt_algorithm_state()) {
            const TspAlgorithmStateContainer::TwoOptAlgorithmStateDto &two_opt_state = dto.two_opt_algorithm_state();
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