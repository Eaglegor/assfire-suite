#pragma once

#include <vector>
#include "assfire/tsp/api/TspValidationResult.hpp"
#include "assfire/tsp/api/TspPoint.hpp"

namespace assfire::tsp {
    class TspValidator {
    public:
        using Points = std::vector<TspPoint>;
        using Sequence = std::vector<int>;

        virtual TspValidationResult validate(const Sequence& sequence) const = 0;
    };
}