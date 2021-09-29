#pragma once

#include <string>

namespace assfire::tsp {
    enum class TspValidatorType {
        DEFAULT,
        FAIL_ON_ANY_VIOLATION
    };
}

namespace std {
    static std::string to_string(const assfire::tsp::TspValidatorType& type) {
        switch(type) {
            case assfire::tsp::TspValidatorType::DEFAULT:
                return "DEFAULT";
            case assfire::tsp::TspValidatorType::FAIL_ON_ANY_VIOLATION:
                return "FAIL_ON_ANY_VIOLATION";
            default:
                return "<UNKNOWN>";
        }
    }
}