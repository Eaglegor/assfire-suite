#pragma once

#include <cstdint>
#include <cmath>
#include <spdlog/spdlog.h>

#include <assfire/api/v1/model/routing/routing.pb.h>

namespace assfire::router
{
    class CoordinatesEncoder
    {
    public:
        using CoordinateFormat = assfire::api::v1::model::routing::CoordinateFormat;
        constexpr static int FIXED_POINT_INT_PRECISION = 6;

        CoordinatesEncoder() = delete;

        static double encode(double coordinate, const CoordinateFormat &format) noexcept
        {
            switch (format) {
                case api::v1::model::routing::COORDINATE_FORMAT_FIXED_POINT_INT:
                    return convertToFixedPointIntRepresentation(coordinate, format);
                default:
                    return processUnknownRepresentation(coordinate, format);
            }
        }

    private:
        static std::int32_t convertToFixedPointIntRepresentation(double coordinate, const CoordinateFormat &format) noexcept
        {
            return static_cast<std::int32_t>(coordinate * std::pow(10, FIXED_POINT_INT_PRECISION));
        }

        static double processUnknownRepresentation(std::int32_t coordinate, const CoordinateFormat &format) noexcept
        {
            SPDLOG_ERROR("Unknown, unset or unsupported coordinates format: {}", format);
            return 0;
        }
    };
}