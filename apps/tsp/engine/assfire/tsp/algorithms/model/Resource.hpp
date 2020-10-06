#pragma once

#include "assfire/api/v1/model/optimization/transport/transport.pb.h"
#include "RoutingOptions.hpp"

namespace assfire::tsp
{
    struct Resource
    {
        using Data = assfire::api::v1::model::optimization::transport::Resource;

        Resource(const RoutingOptions &routing_options, const Data &data)
                : routing_options(routing_options), data(data)
        {}

        const RoutingOptions routing_options;
        const Data &data;
    };
}