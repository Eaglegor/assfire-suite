#pragma once

#include <vector>
#include <memory>
#include "TspPoint.hpp"
#include "TspCost.hpp"
#include "TspValidationResult.hpp"

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

        TspCost calculateCost(const Points& points, const Sequence& sequence);
        TspValidationResult validate(const Points& points, const Sequence& sequence);

    private:
        ValidatorPtr validator;
        CostFunctionPtr cost_function;
    };
}