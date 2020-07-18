#pragma once

#include <vector>
#include <string>
#include <memory>
#include "../DistanceMatrixBackend.hpp"
#include "../settings/RouteRequestSettings.hpp"

namespace assfire
{
	class GrpcClient;

	class DirectRequestBackend : public DistanceMatrixBackend
	{
	public:
		DirectRequestBackend(
			const std::vector<Location>& locations,
			const RouteRequestSettings& settings,
			const std::string& backend_host,
			int backend_port
		);

		virtual ~DirectRequestBackend();

		virtual RouteInfo getRoute(const Location& origin, const Location& destination) const override;
		virtual RouteInfo getRoute(long origin_id, const Location& destination) const override;
		virtual RouteInfo getRoute(const Location& origin, long destination_id) const override;
		virtual RouteInfo getRoute(long origin_id, long destination_id) const override;
		virtual long getId(const Location& location) const override;

	private:
		std::vector<Location> locations;
		std::unique_ptr<GrpcClient> client;
	};
}