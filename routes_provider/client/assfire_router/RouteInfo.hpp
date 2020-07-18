#pragma once

namespace assfire
{
	class RouteInfo
	{
	public:
		RouteInfo(double distance, long duration):
			distance(distance),
			duration(duration)
		{
		}

		double getDistance() const {
			return distance;
		}

		double getDuration() const {
			return duration;
		}

	private:
		double distance;
		long duration;
	};
}