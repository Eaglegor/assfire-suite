#pragma once

namespace assfire::tsp {
    enum class TspAlgorithmType {
        AUTO,
        TWO_OPT
    };
}

namespace std {
    static std::string to_string(assfire::tsp::TspAlgorithmType value) {
        switch (value) {
            case assfire::tsp::TspAlgorithmType::AUTO:
                return "AUTO";
            case assfire::tsp::TspAlgorithmType::TWO_OPT:
                return "TWO_OPT";
            default:
                return "<UNKNOWN>";
        }
    }
}