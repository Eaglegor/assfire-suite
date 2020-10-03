#pragma once

#include <cstdint>
#include <cmath>
#include <spdlog/spdlog.h>

#include "assfire/routing/v1/routing.pb.h"

namespace assfire {
    class CoordinatesEncoder
    {
    public:
        CoordinatesEncoder() = delete;

        static double encode(double coordinate, const routing::proto::v1::CoordinateFormat &format) noexcept
        {
            switch (format.type()) {
                case routing::proto::v1::CoordinateFormat::FIXED_POINT_INT:
                    return convertToFixedPointIntRepresentation(coordinate, format);
                default:
                    return processUnknownRepresentation(coordinate, format);
            }
        }

    private:
        static std::int32_t convertToFixedPointIntRepresentation(double coordinate,
                                                         const routing::proto::v1::CoordinateFormat &format) noexcept
        {
            return static_cast<std::int32_t>(coordinate * std::pow(10, format.precision()));
        }

        static double processUnknownRepresentation(std::int32_t coordinate, const routing::proto::v1::CoordinateFormat &format) noexcept
        {
            SPDLOG_ERROR("Unknown, unset or unsupported coordinates format: {}", format.DebugString());
            return 0;
        }
    };
}