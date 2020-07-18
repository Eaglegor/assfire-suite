#pragma once

#include <routing.pb.h>
#include <future>

namespace assfire
{
	class Router
	{
	public:
		virtual ~Router() {}
		virtual routing::proto::RouteInfo getRoute(routing::proto::SingleRouteRequest, long) const = 0;
		virtual void getRoutesBatch(routing::proto::ManyToManyRoutesRequest, std::function<void(routing::proto::RouteInfo)>, long) const = 0;
	};
}