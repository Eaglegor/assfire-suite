#pragma once

#include <vector>
#include "TspPoint.hpp"
#include "TspSolverSettings.hpp"

namespace assfire::tsp
{
    class TspEstimator;

    class TspTask
    {
    public:
        using Points = std::vector<TspPoint>;

        TspTask(const Points &points, const TspSolverSettings &solver_settings) : points(points), solver_settings(solver_settings) {}

        const Points &getPoints() const {
            return points;
        }

        const TspSolverSettings &getSolverSettings() const {
            return solver_settings;
        }

    private:
        Points points;
        TspSolverSettings solver_settings;
    };
}