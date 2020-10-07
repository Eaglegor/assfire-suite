#pragma once

#include "assfire/api/v1/model/optimization/transport/transport.pb.h"
#include "RoutingOptions.hpp"

namespace assfire::tsp
{
    class Resource
    {
    public:
        using Data = assfire::api::v1::model::optimization::transport::Resource;

        Resource(const RoutingOptions &routing_options, const Data &data)
                : routing_options(routing_options), data(&data)
        {}

        const RoutingOptions &getRoutingOptions() const
        {
            return routing_options;
        }

        const Data &getData() const
        {
            return *data;
        }

    private:
        const RoutingOptions routing_options;
        const Data *data;
    };
}