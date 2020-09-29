#pragma once

#include <variant>
#include "EuclideanSettings.hpp"
#include "RandomSettings.hpp"
#include "FixedPointIntCoordinatesFormat.hpp"

namespace assfire
{
	struct RouteRequestSettings
	{
		std::variant<FixedPointIntCoordinatesFormat> coordinates_format;
		std::variant<EuclideanSettings, RandomSettings> calculation_settings;
	};
}