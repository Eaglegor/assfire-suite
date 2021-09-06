#pragma once

#include <vector>
#include "TspEstimator.hpp"
#include "TspPoint.hpp"
#include "assfire/router/api/DistanceMatrix.hpp"

namespace assfire::tsp
{
    class TspEstimator;

    class TspTask
    {
    public:
        using Points = std::vector<TspPoint>;

        TspTask(TspTask::Points points, TspEstimator estimator, const router::DistanceMatrix &distance_matrix);

        const TspEstimator &getEstimator() const;

        const Points &getPoints() const;

        const router::DistanceMatrix &getDistanceMatrix() const;

    private:
        TspEstimator estimator;
        Points points;
        router::DistanceMatrix distance_matrix;
    };
}