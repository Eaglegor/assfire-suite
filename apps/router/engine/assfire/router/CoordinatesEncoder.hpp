#pragma once

#include <cstdint>
#include <cmath>
#include <spdlog/spdlog.h>

#include <assfire/api/v1/model/routing/routing.pb.h>

namespace assfire::router {
    class CoordinatesEncoder
    {
    public:
        using CoordinateFormat = assfire::api::v1::model::routing::CoordinateFormat;

        CoordinatesEncoder() = delete;

        static double encode(double coordinate, const CoordinateFormat &format) noexcept
        {
            switch (format.type()) {
                case CoordinateFormat::FIXED_POINT_INT:
                    return convertToFixedPointIntRepresentation(coordinate, format);
                default:
                    return processUnknownRepresentation(coordinate, format);
            }
        }

    private:
        static std::int32_t convertToFixedPointIntRepresentation(double coordinate, const CoordinateFormat &format) noexcept
        {
            return static_cast<std::int32_t>(coordinate * std::pow(10, format.precision()));
        }

        static double processUnknownRepresentation(std::int32_t coordinate, const CoordinateFormat &format) noexcept
        {
            SPDLOG_ERROR("Unknown, unset or unsupported coordinates format: {}", format.DebugString());
            return 0;
        }
    };
}