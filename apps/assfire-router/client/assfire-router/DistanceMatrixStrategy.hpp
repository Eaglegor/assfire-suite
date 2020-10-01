#pragma once

#include "RouteInfo.hpp"
#include "Location.hpp"

namespace assfire
{
	class DistanceMatrixStrategy
	{
	public:
		virtual ~DistanceMatrixStrategy() {}

		virtual RouteInfo getRoute(const Location& origin, const Location& destination) const = 0;
		virtual RouteInfo getRoute(long origin_id, const Location& destination) const = 0;
		virtual RouteInfo getRoute(const Location& origin, long destination_id) const = 0;
		virtual RouteInfo getRoute(long origin_id, long destination_id) const = 0;
		virtual long getId(const Location& location) const = 0;
	};
}