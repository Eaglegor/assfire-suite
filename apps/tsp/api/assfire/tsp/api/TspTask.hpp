#pragma once
#include <vector>
#include "TspEstimator.hpp"
#include "TspPoint.hpp"

namespace assfire::tsp {
    class TspEstimator;

    class TspTask {
    public:
        using Points = std::vector<TspPoint>;

        TspTask(const Points& points, const TspEstimator& estimator);

        const TspEstimator &getEstimator() const;

        const Points &getPoints() const;

    private:
        TspEstimator estimator;
        Points points;
    };
}