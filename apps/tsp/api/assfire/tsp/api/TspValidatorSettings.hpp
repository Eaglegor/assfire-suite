#pragma once

#include "TspValidatorType.hpp"

namespace assfire::tsp {
    class TspValidatorSettings {
    public:
        TspValidatorType getType() const {
            return type;
        }

        void setType(TspValidatorType type) {
            TspValidatorSettings::type = type;
        }

    private:
        TspValidatorType type;
    };
}