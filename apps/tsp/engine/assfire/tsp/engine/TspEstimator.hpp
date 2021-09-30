#pragma once

#include <vector>
#include <memory>
#include "assfire/tsp/api/TspPoint.hpp"
#include "assfire/tsp/api/TspCost.hpp"
#include "assfire/tsp/api/TspValidationResult.hpp"

namespace assfire::tsp
{
    class TspCostFunction;
    class TspValidator;

    class TspEstimator
    {
    public:
        using Points = std::vector<TspPoint>;
        using Sequence = std::vector<int>;
        using ValidatorPtr = std::shared_ptr<TspValidator>;
        using CostFunctionPtr = std::shared_ptr<TspCostFunction>;

        TspEstimator(ValidatorPtr validator, CostFunctionPtr cost_function);

        TspCost calculateCost(const Points& points, const Sequence& sequence) const;
        TspValidationResult validate(const Points& points, const Sequence& sequence) const;

    private:
        ValidatorPtr validator;
        CostFunctionPtr cost_function;
    };
}