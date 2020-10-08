#pragma once

#include "assfire/api/v1/model/routing/routing.pb.h"
#include "assfire/router/client/DistanceMatrix.hpp"
#include <cstdint>

namespace assfire::tsp
{
    class RoutingOptions
    {
    public:
        using Data = assfire::api::v1::model::routing::RoutingOptions;

        RoutingOptions(std::int32_t id, const Data &data)
                : id(id), data(&data)
        {}

        const int32_t getId() const
        {
            return id;
        }

        const Data &getData() const
        {
            return *data;
        }

        static RoutingOptions of(const Data &data, router::DistanceMatrix &distance_matrix)
        {
            return RoutingOptions(distance_matrix.addRoutingOptions(data), data);
        }

    private:
        const std::int32_t id;
        const Data *data;
    };
}