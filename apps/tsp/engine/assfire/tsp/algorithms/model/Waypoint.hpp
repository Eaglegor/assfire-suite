#pragma once

#include "Job.hpp"

namespace assfire::tsp
{
    struct Waypoint
    {
        explicit Waypoint(const Job &job)
                : job(job)
        {}

        const Job &job;
    };
}