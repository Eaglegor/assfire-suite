#pragma once

#include "assfire/api/v1/model/optimization/transport/transport.pb.h"
#include "Location.hpp"

namespace assfire::tsp
{
    struct Job
    {
        using Data = assfire::api::v1::model::optimization::transport::Job;

        Job(const Location &location, const Data &data)
                : location(location), data(data)
        {}

        const Location location;
        const Data &data;
    };
}