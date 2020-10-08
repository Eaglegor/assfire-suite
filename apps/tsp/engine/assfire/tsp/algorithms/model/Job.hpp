#pragma once

#include "assfire/api/v1/model/optimization/transport/transport.pb.h"
#include "Location.hpp"

namespace assfire::tsp
{
    class Job
    {
    public:
        using Data = assfire::api::v1::model::optimization::transport::Job;

        Job(const Location &location, const Data &data)
                : location(location), data(&data)
        {}

        const Location& getLocation() const{
            return location;
        }
        const Data& getData() const{
            return *data;
        }

    private:
        const Location location;
        const Data *data;
    };
}