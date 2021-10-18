#pragma once

#include "TspCostFunction.hpp"
#include "assfire/router/api/DistanceMatrix.hpp"

namespace assfire::tsp {
    class TotalDistanceTspCostFunction : public TspCostFunction {
    public:
        TotalDistanceTspCostFunction(router::DistanceMatrix distance_matrix, std::vector<router::IndexedLocation> indexed_locations);

        TspCost estimate(const Sequence &sequence) const override;

    private:
        router::DistanceMatrix distance_matrix;
        std::vector<router::IndexedLocation> indexed_locations;
    };
}