#pragma once

#include <assfire/api/v1/tsp/state.pb.h>

namespace assfire::tsp {
    class TspAlgorithmStateContainer {
    public:
        using TspAlgorithmStateDto = assfire::api::v1::tsp::TspAlgorithmState;
        using TwoOptAlgorithmStateDto = assfire::api::v1::tsp::TwoOptAlgorithmState;

        TspAlgorithmStateDto& getDto();
        const TspAlgorithmStateDto& getDto() const;

    private:
        TspAlgorithmStateDto state;
    };
}