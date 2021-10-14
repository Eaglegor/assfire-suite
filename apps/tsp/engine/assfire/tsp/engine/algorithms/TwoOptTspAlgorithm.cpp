#include "TwoOptTspAlgorithm.hpp"
#include "assfire/tsp/api/TspTask.hpp"
#include <vector>
#include <optional>
#include <algorithm>

namespace assfire::tsp {
    TwoOptTspAlgorithm::TwoOptTspAlgorithm(TspEstimator estimator) : estimator(std::move(estimator)) {}

    TspSolution TwoOptTspAlgorithm::createInitialSolution(const TspTask &task, bool is_final) const {
        TspSolution::Sequence initial_sequence;
        for (int i = 0; i < task.getPoints().size(); ++i) {
            initial_sequence.push_back(i);
        }
        return TspSolution(initial_sequence,
                           estimator.calculateCost(task.getPoints(), initial_sequence),
                           estimator.validate(task.getPoints(), initial_sequence),
                           is_final);
    }

    void TwoOptTspAlgorithm::solveTsp(const TspTask &task, EngineTspSolutionController &solution_controller) const {
        TspSolution solution = createInitialSolution(task, false);

        solution_controller.publishSolution(solution);

        TspSolution::Sequence sequence = solution.getSequence();

        std::optional<State> current_iteration_state = createInitialState(sequence.size(), solution_controller.getStateContainer());

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
                saveState(*current_iteration_state, solution_controller.getStateContainer());
            }
        }

        solution_controller.publishSolution(solution.asFinal());
    }

    std::optional<TwoOptTspAlgorithm::State> TwoOptTspAlgorithm::createInitialState(int size, const TspAlgorithmStateContainer &state_container) const {

        std::optional<State> loaded_state = loadState(state_container);
        if (loaded_state) {
            // [TODO] Log loaded state
            return loaded_state;
        }

        return std::make_optional<State>(0, 1, 0, size);
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

    void TwoOptTspAlgorithm::saveState(const TwoOptTspAlgorithm::State &state, TspAlgorithmStateContainer &container) const {
        TspAlgorithmStateContainer::TwoOptAlgorithmStateDto *two_opt_state = container.getDto().mutable_two_opt_algorithm_state();
        two_opt_state->set_i(state.getI());
        two_opt_state->set_j(state.getJ());
        two_opt_state->set_k(state.getK());
        two_opt_state->set_size(state.getSize());
    }

    std::optional<TwoOptTspAlgorithm::State> TwoOptTspAlgorithm::loadState(const TspAlgorithmStateContainer &container) const {
        const TspAlgorithmStateContainer::TspAlgorithmStateDto &dto = container.getDto();
        if (dto.has_two_opt_algorithm_state()) {
            const TspAlgorithmStateContainer::TwoOptAlgorithmStateDto &two_opt_state = dto.two_opt_algorithm_state();
            return std::make_optional<State>(two_opt_state.i(), two_opt_state.j(), two_opt_state.k(), two_opt_state.size());
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