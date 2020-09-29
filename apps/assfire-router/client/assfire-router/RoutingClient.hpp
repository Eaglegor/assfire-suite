#pragma once

#include <vector>
#include "DistanceMatrix.hpp"
#include "backends/DirectRequestBackend.hpp"

namespace assfire
{
	class RoutingClient
	{
	public:
		template<typename Backend, typename... Args>
		DistanceMatrix getDistanceMatrix(const std::vector<Location>& locations,  Args&& ...args) const {
			DistanceMatrix result = DistanceMatrix(std::make_unique<Backend>(locations, std::forward<Args>(args)...));

			return std::move(result);
		}
	};
}