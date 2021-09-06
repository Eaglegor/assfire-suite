#pragma once

#include <vector>
#include "TspPoint.hpp"
#include "TspCost.hpp"

namespace assfire::tsp
{
    class TspCostFunction
    {
    public:
        using Points = std::vector<TspPoint>;
        using Sequence = std::vector<int>;

        virtual TspCost estimate(const Points& points, const Sequence& sequence) const = 0;
    };
}