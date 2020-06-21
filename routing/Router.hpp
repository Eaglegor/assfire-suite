#pragma once

#include <routing.pb.h>
#include <future>

namespace assfire
{
	class Router
	{
	public:
		virtual routing::RouteInfo getRoute(routing::SingleRouteRequest) const = 0;
		virtual void getRoutesBatch(routing::ManyToManyRoutesRequest, std::function<void(routing::RouteInfo)>) const = 0;
	};
}