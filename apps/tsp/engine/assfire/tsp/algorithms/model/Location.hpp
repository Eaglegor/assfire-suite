#pragma once

#include "assfire/api/v1/model/routing/routing.pb.h"
#include <cstdint>

namespace assfire::tsp
{
    class Location
    {
    public:
        using Data = assfire::api::v1::model::routing::Location;

        Location(std::int32_t id, const Data &data)
                : id(id), data(&data)
        {}

        const int32_t& getId() const
        {
            return id;
        }

        const Data& getData() const
        {
            return *data;
        }

        static Location of(const Data &data, router::DistanceMatrix &distance_matrix)
        {
            return Location(distance_matrix.addLocation(data), data);
        }

    private:
        const std::int32_t id;
        const Data *data;
    };
}