#include <iostream>

#include "assfire/tsp/api/TspAlgorithmType.hpp"
#include "assfire/tsp/api/TspCostFunctionType.hpp"
#include "assfire/tsp/api/TspValidatorType.hpp"
#include "assfire/tsp/api/TspCost.hpp"

namespace assfire::tsp {
    std::ostream& operator<<(std::ostream& str, const TspAlgorithmType& value);
    std::ostream& operator<<(std::ostream& str, const TspCostFunctionType& value);
    std::ostream& operator<<(std::ostream& str, const TspValidatorType& value);
    std::ostream& operator<<(std::ostream& str, const TspCost& value);
}
