#pragma once

#include "Job.hpp"
#include <functional>

namespace assfire::tsp
{
    class Waypoint
    {
    public:
        explicit Waypoint(const Job &job)
                : job(&job)
        {}

        const Job& getJob() const {
            return *job;
        }

    private:
        const Job *job;
    };
}