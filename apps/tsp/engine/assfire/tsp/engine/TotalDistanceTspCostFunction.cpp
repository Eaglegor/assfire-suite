#include "TotalDistanceTspCostFunction.hpp"

#include <utility>

namespace assfire::tsp {
    TotalDistanceTspCostFunction::TotalDistanceTspCostFunction(router::DistanceMatrix distance_matrix, std::vector<router::IndexedLocation> indexed_locations)
            : distance_matrix(std::move(distance_matrix)), indexed_locations(std::move(indexed_locations)) {}

    TspCost TotalDistanceTspCostFunction::estimate(const TspCostFunction::Sequence &sequence) const {
        TspCost::value_type cost = 0;
        for (int i = 0; i < sequence.size() - 1; ++i) {
            cost += distance_matrix.getRouteInfo(indexed_locations[sequence[i]], indexed_locations[sequence[i + 1]]).getDistance().toMeters();
        }
        return TspCost(cost);
    }
}