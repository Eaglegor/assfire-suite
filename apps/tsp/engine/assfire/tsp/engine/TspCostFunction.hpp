#pragma once

#include <vector>
#include "assfire/tsp/api/TspPoint.hpp"
#include "assfire/tsp/api/TspCost.hpp"

namespace assfire::tsp
{
    class TspCostFunction
    {
    public:
        using Points = std::vector<TspPoint>;
        using Sequence = std::vector<int>;

        virtual TspCost estimate(const Sequence& sequence) const = 0;
    };
}