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
    struct Solution
    {
        explicit Solution(std::vector<Waypoint> waypoints, const Problem &problem)
                : waypoints(std::move(waypoints)), problem(problem)
        {}

        explicit Solution(const Problem &problem)
                : problem(problem)
        {
            std::transform(problem.jobs.begin(), problem.jobs.end(), std::back_inserter(waypoints), [](const Job &j) {
                return Waypoint(j);
            });
        }

        Solution &operator=(const Solution &rhs)
        {
            assert(&rhs.problem == &problem);
            this->waypoints = rhs.waypoints;
        };

        assfire::api::v1::service::tsp::TspSolution toTspSolution()
        {
            assfire::api::v1::service::tsp::TspSolution result;

            for (const Waypoint &wp : waypoints) {
                assfire::api::v1::model::optimization::transport::Waypoint *waypoint = result.add_waypoints();
                waypoint->mutable_job()->CopyFrom(wp.job.data);
            }

            return result;
        }

        std::vector<Waypoint> waypoints;
        const Problem &problem;
    };
}