#include "TwoOptTspAlgorithm.hpp"
#include "assfire/tsp/api/TspTask.hpp"
#include <vector>
#include <optional>
#include <algorithm>

namespace assfire::tsp
{
    static TspSolution createInitialSolution(const TspTask &task, bool is_final)
    {
        TspSolution::Sequence initial_sequence;
        for (int i = 0; i < task.getPoints().size(); ++i) {
            initial_sequence.push_back(i);
        }
        return TspSolution(initial_sequence,
                           task.getEstimator().calculateCost(task.getPoints(), initial_sequence),
                           task.getEstimator().validate(task.getPoints(), initial_sequence),
                           is_final);
    }

    TspSolution TwoOptTspAlgorithm::solveTsp(const TspTask &task, const TspInterruptor &interruptor) const
    {
        std::optional<TspSolution> result;

        solveTsp(task,
                 [&](const TspSolution &intermediateSolution) {
                     result = intermediateSolution;
                 },
                 interruptor);

        if (result) {
            return result->asFinal();
        } else {
            return createInitialSolution(task, true);
        }
    }

    void TwoOptTspAlgorithm::solveTsp(const TspTask &task, TspResultCallback resultCallback, const TspInterruptor &interruptor) const
    {
        TspSolution solution = createInitialSolution(task, false);

        resultCallback(solution);

        TspSolution::Sequence sequence = solution.getSequence();

        const TspEstimator& estimator = task.getEstimator();

        for (int k = 0; k < sequence.size() && !interruptor.isInterrupted(); ++k) {
            for (int i = 0; i < sequence.size() && !interruptor.isInterrupted(); ++i) {
                for (int j = i + 1; j < sequence.size() && !interruptor.isInterrupted(); ++j) {
                    std::reverse(sequence.begin() + i, sequence.begin() + j);
                    TspValidationResult validation_result = estimator.validate(task.getPoints(), sequence);
                    if(validation_result.isValid()) {
                        TspCost cost = estimator.calculateCost(task.getPoints(), sequence);
                        if(cost < solution.getCost()) {
                            resultCallback(TspSolution(sequence, cost, validation_result, false));
                        } else {
                            std::reverse(sequence.begin() + i, sequence.begin() + j);
                        }
                    } else {
                        std::reverse(sequence.begin() + i, sequence.begin() + j);
                    }
                }
            }
        }

        resultCallback(solution.asFinal());
    }
}