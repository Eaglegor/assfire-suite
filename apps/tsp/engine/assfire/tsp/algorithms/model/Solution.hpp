#pragma once

#include <utility>
#include <vector>
#include <algorithm>
#include <iterator>
#include "Waypoint.hpp"
#include "Problem.hpp"
#include "assert.h"

namespace assfire::tsp
{
    class Solution
    {
    public:
        explicit Solution(std::vector<Waypoint> waypoints, const Problem &problem)
                : waypoints(std::move(waypoints)), problem(&problem)
        {}

        explicit Solution(const Problem &problem)
                : problem(&problem)
        {
            std::transform(problem.getJobs().begin(), problem.getJobs().end(), std::back_inserter(waypoints), [](const Job &j) {
                return Waypoint(j);
            });
        }

        const std::vector<Waypoint> &getWaypoints() const
        {
            return waypoints;
        }

        std::vector<Waypoint> &getWaypoints()
        {
            return waypoints;
        }

        const Problem &getProblem() const
        {
            return *problem;
        }

        assfire::api::v1::service::tsp::TspSolution toTspSolution() const
        {
            assfire::api::v1::service::tsp::TspSolution result;

            for (const Waypoint &wp : waypoints) {
                assfire::api::v1::model::optimization::transport::Waypoint *waypoint = result.add_waypoints();
                waypoint->mutable_job()->CopyFrom(wp.getJob().getData());
            }

            return result;
        }

    private:
        std::vector<Waypoint> waypoints;
        const Problem *problem;
    };
}