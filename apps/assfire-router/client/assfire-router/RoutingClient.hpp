#pragma once

#include <vector>
#include "DistanceMatrix.hpp"
#include "strategies/DirectRequestStrategy.hpp"

namespace assfire
{
	class RoutingClient
	{
	public:
		template<typename Strategy, typename... Args>
		DistanceMatrix getDistanceMatrix(const std::vector<Location>& locations,  Args&& ...args) const {
			DistanceMatrix result = DistanceMatrix(std::make_unique<Strategy>(locations, std::forward<Args>(args)...));

			return std::move(result);
		}
	};
}