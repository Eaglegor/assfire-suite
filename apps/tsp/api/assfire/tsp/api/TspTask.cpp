#include "TspTask.hpp"

#include <utility>

namespace assfire::tsp
{

    TspTask::TspTask(TspTask::Points points, TspEstimator estimator, const router::DistanceMatrix &distance_matrix)
            : points(std::move(points)),
              estimator(std::move(estimator)),
              distance_matrix(distance_matrix)
    {

    }

    const TspEstimator &TspTask::getEstimator() const
    {
        return estimator;
    }

    const TspTask::Points &TspTask::getPoints() const
    {
        return points;
    }

    const router::DistanceMatrix &TspTask::getDistanceMatrix() const
    {
        return distance_matrix;
    }
}