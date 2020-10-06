#pragma once

#include "assfire/api/v1/model/routing/routing.pb.h"
#include <cstdint>

namespace assfire::tsp
{
    struct Location
    {
        using Data = assfire::api::v1::model::routing::Location;

        Location(std::int32_t id, const Data &data)
                : id(id), data(data)
        {}

        const std::int32_t id;
        const Data &data;

        static Location of(const Data& data, router::DistanceMatrix& distance_matrix){
            return Location(distance_matrix.addLocation(data), data);
        }
    };
}