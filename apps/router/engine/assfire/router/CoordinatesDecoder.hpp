#pragma once

#include <cstdint>
#include <cmath>
#include <spdlog/spdlog.h>

#include <assfire/api/v1/model/routing/routing.pb.h>

namespace assfire::router {
    class CoordinatesDecoder
    {
    public:
        using CoordinateFormat = assfire::api::v1::model::routing::CoordinateFormat;

        CoordinatesDecoder() = delete;

        static double decode(std::int32_t coordinate, const CoordinateFormat &format) noexcept
        {
            switch (format.type()) {
                case CoordinateFormat::FIXED_POINT_INT:
                    return convertFixedPointIntRepresentation(coordinate, format);
                default:
                    return processUnknownRepresentation(coordinate, format);
            }
        }

    private:
        static double convertFixedPointIntRepresentation(std::int32_t coordinate, const CoordinateFormat &format) noexcept
        {
            return coordinate * std::pow(0.1, format.precision());
        }

        static double processUnknownRepresentation(std::int32_t coordinate, const CoordinateFormat &format) noexcept
        {
            SPDLOG_ERROR("Unknown, unset or unsupported coordinates format: {} ({})", coordinate, format.DebugString());
            return 0;
        }
    };
}