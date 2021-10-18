#include "TspEstimator.hpp"

#include <utility>
#include "TspValidator.hpp"
#include "TspCostFunction.hpp"

namespace assfire::tsp
{

    TspEstimator::TspEstimator(ValidatorPtr validator, CostFunctionPtr cost_function)
            : validator(std::move(validator)), cost_function(std::move(cost_function))
    {}

    TspCost TspEstimator::calculateCost(const Sequence &sequence) const
    {
        return cost_function->estimate(sequence);
    }

    TspValidationResult TspEstimator::validate(const Sequence &sequence) const
    {
        return validator->validate(sequence);
    }
}