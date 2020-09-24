#pragma once

#include <cstdint>
#include <cmath>

#include <assfire/routing/v1/routing.pb.h>

namespace assfire {
    class CoordinatesDecoder
    {
    public:
        CoordinatesDecoder() = delete;

        static double decode(std::int32_t coordinate, const routing::proto::v1::CoordinateFormat &format) noexcept
        {
            switch (format.type()) {
                case routing::proto::v1::CoordinateFormat::FIXED_POINT_INT:
                    return convertFixedPointIntRepresentation(coordinate, format);
                default:
                    return fallbackToDefaultRepresentation(coordinate);
            }
        }

    private:
        static double convertFixedPointIntRepresentation(std::int32_t coordinate,
                                                         const routing::proto::v1::CoordinateFormat &format) noexcept
        {
            return coordinate * std::pow(0.1, format.precision());
        }

        static double fallbackToDefaultRepresentation(std::int32_t coordinate) noexcept
        {
            routing::proto::v1::CoordinateFormat default_format;
            default_format.set_type(routing::proto::v1::CoordinateFormat::FIXED_POINT_INT);
            default_format.set_precision(6);
            return convertFixedPointIntRepresentation(coordinate, default_format);
        }
    };
}