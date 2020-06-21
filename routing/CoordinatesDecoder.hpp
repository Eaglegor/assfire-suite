#pragma once

#include <cstdint>
#include <cmath>

#include <routing.pb.h>

namespace assfire
{
	class CoordinatesDecoder {
	public:
		CoordinatesDecoder() = delete;

		static double decode(std::int32_t coordinate, const routing::CoordinateFormat& format) noexcept
		{
			if (format.has_fixed_point_int()) return convertFixedPointIntRepresentation(coordinate, format.fixed_point_int());
			else return fallbackToDefaultRepresentation(coordinate);
		}

	private:
		static double convertFixedPointIntRepresentation(std::int32_t coordinate, const routing::CoordinateFormat::FixedPointInt& format) noexcept
		{
			return coordinate * std::pow(0.1, format.precision());
		}

		static double fallbackToDefaultRepresentation(std::int32_t coordinate) noexcept
		{
			routing::CoordinateFormat::FixedPointInt default_format;
			default_format.set_precision(6);
			return convertFixedPointIntRepresentation(coordinate, default_format);
		}
	};
}