#pragma once
#include "assfire/router/api/DistanceMatrix.hpp"

namespace assfire::scheduler {
    class WaybillSchedulingContext {
    public:
        WaybillSchedulingContext(const router::DistanceMatrix &distance_matrix) : distance_matrix(distance_matrix) {}

        const router::DistanceMatrix& getDistanceMatrix() const
        {
            return distance_matrix;
        }

    private:
        router::DistanceMatrix distance_matrix;
    };
}
