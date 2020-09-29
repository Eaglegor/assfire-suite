#include "DirectRequestBackend.hpp"
#include "../grpc_client/GrpcClient.hpp"
#include <algorithm>

using namespace assfire;

DirectRequestBackend::DirectRequestBackend(const std::vector<Location>& locations, const RouteRequestSettings& settings, const std::string& backend_host, int backend_port, bool use_ssl):
	locations(locations),
	client(std::make_unique<GrpcClient>(backend_host, backend_port, settings, use_ssl))
{
}

assfire::DirectRequestBackend::~DirectRequestBackend()
{
}

RouteInfo DirectRequestBackend::getRoute(const Location& origin, const Location& destination) const
{
	return client->getSingleRoute(origin, destination);
}

RouteInfo DirectRequestBackend::getRoute(long origin_id, const Location& destination) const
{
	return client->getSingleRoute(locations[origin_id], destination);
}

RouteInfo DirectRequestBackend::getRoute(const Location& origin, long destination_id) const
{
	return client->getSingleRoute(origin, locations[destination_id]);
}

RouteInfo DirectRequestBackend::getRoute(long origin_id, long destination_id) const
{
	return client->getSingleRoute(locations[origin_id], locations[destination_id]);
}

long DirectRequestBackend::getId(const Location& location) const
{
	auto iter = std::find(locations.begin(), locations.end(), location);
	return iter == locations.end() ? -1 : std::distance(locations.begin(), iter);
}
