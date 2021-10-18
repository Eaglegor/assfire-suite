#include <assfire/tsp/api/TspValidationResult.hpp>
#include "NopTspValidator.hpp"

namespace assfire::tsp {
    TspValidationResult tsp::NopTspValidator::validate(const Sequence &sequence) const {
        return TspValidationResult(true);
    }
}
