#pragma once

#include <vector>
#include "TspCost.hpp"
#include "TspValidationResult.hpp"

namespace assfire::tsp
{
    class TspSolution
    {
    public:
        using Sequence = std::vector<int>;

        TspSolution(const Sequence &sequence, const TspCost &cost, const TspValidationResult &validation_result, bool is_final_solution)
                : sequence(sequence), cost(cost), validation_result(validation_result), is_final_solution(is_final_solution)
        {}

        TspSolution &operator=(const TspSolution &) = default;

        TspSolution asFinal()
        {
            return TspSolution(sequence, cost, validation_result, true);
        }

        const Sequence &getSequence() const
        {
            return sequence;
        }

        const TspCost &getCost() const
        {
            return cost;
        }

        const TspValidationResult &getValidationResult() const
        {
            return validation_result;
        }

        bool isFinalSolution() const
        {
            return is_final_solution;
        }

    private:
        Sequence sequence;
        TspCost cost;
        TspValidationResult validation_result;
        bool is_final_solution;
    };
}