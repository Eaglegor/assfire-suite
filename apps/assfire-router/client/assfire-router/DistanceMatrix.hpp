#pragma once
#include <memory>
#include "DistanceMatrixStrategy.hpp"
#include "RouteInfo.hpp"

namespace assfire
{
	/**
	 * Provides an interface to retrieve routes from the underlying routing service
	 */
	class DistanceMatrix
	{
	public:
		DistanceMatrix(std::unique_ptr<DistanceMatrixStrategy>&& strategy):
                strategy(std::forward<std::unique_ptr<DistanceMatrixStrategy>>(strategy))
		{}

		/**
		 * Retrieves route information from underlying routing service
		 * If unknown location is passed (that isn't stored in this distance matrix) then 
		 * the behavior depends on the selected backend
		 *
		 * @param origin - origin coordinates
		 * @param destination - destination coordinates
		 */
		RouteInfo getRoute(const Location& origin, const Location& destination) const
		{
			return strategy->getRoute(origin, destination);
		}

		/**
		 * Retrieves route information from underlying routing service
		 * If unknown location is passed (that isn't stored in this distance matrix) then
		 * the behavior depends on the selected backend
		 *
		 * @param origin_id - origin id in current distance matrix (see getId())
		 * @param destination - destination coordinates
		 */
		RouteInfo getRoute(long origin_id, const Location& destination) const
		{
			return strategy->getRoute(origin_id, destination);
		}

		/**
		 * Retrieves route information from underlying routing service
		 * If unknown location is passed (that isn't stored in this distance matrix) then
		 * the behavior depends on the selected backend
		 *
		 * @param origin - origin coordinates
		 * @param destination - destination id in current distance matrix (see getId())
		 */
		RouteInfo getRoute(const Location& origin, long destination_id) const
		{
			return strategy->getRoute(origin, destination_id);
		}

		/**
		 * Retrieves route information from underlying routing service

		 * @param origin_id - origin id in current distance matrix (see getId())
		 * @param destination_id - destination id in current distance matrix (see getId())
		 */
		RouteInfo getRoute(long origin_id, long destination_id) const
		{
			return strategy->getRoute(origin_id, destination_id);
		}

		/**
		 * Retrieves id of the specified location in current distance matrix
		 *
		 * When distance matrix is created, locations may be rearranged to achieve optimal requests order
		 * This method helps to get the internal id used to represent location.
		 *
		 * @param location - coordinates of location
		 * @returns - non-negative identifier of the specified location. If location is unknown, the behavior depends on the selected backend.
		 */
		long getId(const Location& location) const
		{
			return strategy->getId(location);
		}

	private:
		std::unique_ptr<DistanceMatrixStrategy> strategy;
	};
}