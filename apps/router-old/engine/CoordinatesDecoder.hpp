#pragma once

#include <cstdint>
#include <cmath>
#include <spdlog/spdlog.h>

#include "assfire/routing/v1/routing.pb.h"

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
                    return processUnknownRepresentation(coordinate, format);
            }
        }

    private:
        static double convertFixedPointIntRepresentation(std::int32_t coordinate,
                                                         const routing::proto::v1::CoordinateFormat &format) noexcept
        {
            return coordinate * std::pow(0.1, format.precision());
        }

        static double processUnknownRepresentation(std::int32_t coordinate, const routing::proto::v1::CoordinateFormat &format) noexcept
        {
            SPDLOG_ERROR("Unknown, unset or unsupported coordinates format: {}", format.DebugString());
            return 0;
        }
    };
}