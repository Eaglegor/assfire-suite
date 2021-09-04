#include "TspTask.hpp"

namespace assfire::tsp
{

    TspTask::TspTask(const TspTask::Points &points, const TspEstimator &estimator)
            : points(points),
              estimator(estimator)
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
}