#pragma once

#include "TspValidator.hpp"

namespace assfire::tsp {
    class NopTspValidator : public TspValidator {
    public:
        TspValidationResult validate(const Sequence &sequence) const override;
    };
}