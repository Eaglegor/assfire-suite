#include "Streams.hpp"

namespace assfire::tsp {
    std::ostream &operator<<(std::ostream &str, const TspAlgorithmType &value) {
        switch (value) {
            case TspAlgorithmType::AUTO:
                return str << "AUTO";
            case TspAlgorithmType::TWO_OPT:
                return str << "TWO_OPT";
            case TspAlgorithmType::TIME_WASTING:
                return str << "TIME_WASTING";
            default:
                return str << "<UNKNOWN>";
        }
    }

    std::ostream &operator<<(std::ostream &str, const TspCost &value) {
        return str << value.getValue();
    }

    std::ostream &operator<<(std::ostream &str, const TspCostFunctionType &value) {
        switch (value) {
            case TspCostFunctionType::DEFAULT:
                return str << "DEFAULT";
            case TspCostFunctionType::TOTAL_DISTANCE:
                return str << "TOTAL_DISTANCE";
            default:
                return str << "<UNKNOWN>";
        }
    }

    std::ostream &operator<<(std::ostream &str, const TspValidatorType &value) {
        switch(value) {
            case TspValidatorType::DEFAULT:
                return str << "DEFAULT";
            case TspValidatorType::HARD_CONSTRAINTS:
                return str << "HARD_CONSTRAINTS";
            default:
                return str << "<UNKNOWN>";
        }
    }
}