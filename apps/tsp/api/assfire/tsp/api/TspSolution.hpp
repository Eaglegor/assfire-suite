#pragma once

#include <vector>
#include "TspCost.hpp"
#include "TspValidationResult.hpp"

namespace assfire::tsp {
    class TspSolution {
    public:
        using Sequence = std::vector<int>;

        TspSolution(const Sequence &sequence, const TspCost &cost, const TspValidationResult &validation_result)
                : sequence(sequence), cost(cost), validation_result(validation_result)
        {}

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

    private:
        Sequence sequence;
        TspCost cost;
        TspValidationResult validation_result;
    };
}