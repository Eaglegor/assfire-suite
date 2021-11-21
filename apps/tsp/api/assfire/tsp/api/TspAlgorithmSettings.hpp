#pragma once

#include "TspAlgorithmType.hpp"

namespace assfire::tsp {
    class TspAlgorithmSettings {
    public:
        TspAlgorithmSettings() : algorithm_type(TspAlgorithmType::AUTO) {}

        explicit TspAlgorithmSettings(TspAlgorithmType algorithm_type) : algorithm_type(algorithm_type) {}

        TspAlgorithmType getAlgorithmType() const {
            return algorithm_type;
        }

    private:
        TspAlgorithmType algorithm_type;
    };
}