#pragma once

#include <vector>
#include "TspValidationResult.hpp"
#include "TspPoint.hpp"

namespace assfire::tsp {
    class TspValidator {
    public:
        using Points = std::vector<TspPoint>;
        using Sequence = std::vector<int>;

        virtual TspValidationResult validate(const Points& points, const Sequence& sequence) const = 0;
    };
}