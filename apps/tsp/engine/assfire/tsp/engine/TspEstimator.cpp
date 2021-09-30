#include "TspEstimator.hpp"

#include <utility>
#include "assfire/tsp/api/TspValidator.hpp"
#include "assfire/tsp/api/TspCostFunction.hpp"

namespace assfire::tsp
{

    TspEstimator::TspEstimator(ValidatorPtr validator, CostFunctionPtr cost_function)
            : validator(std::move(validator)), cost_function(std::move(cost_function))
    {}

    TspCost TspEstimator::calculateCost(const Points &points, const Sequence &sequence) const
    {
        return cost_function->estimate(points, sequence);
    }

    TspValidationResult TspEstimator::validate(const Points &points, const Sequence &sequence) const
    {
        return validator->validate(points, sequence);
    }
}