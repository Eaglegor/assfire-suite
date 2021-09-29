#pragma once

namespace assfire::tsp {
    enum class TspCostFunctionType {
        DEFAULT,
        TOTAL_DISTANCE
    };
}

namespace std {
    static std::string to_string(assfire::tsp::TspCostFunctionType value) {
        switch (value) {
            case assfire::tsp::TspCostFunctionType::DEFAULT:
                return "DEFAULT";
            case assfire::tsp::TspCostFunctionType::TOTAL_DISTANCE:
                return "TOTAL_DISTANCE";
            default:
                return "<UNKNOWN>";
        }
    }
}