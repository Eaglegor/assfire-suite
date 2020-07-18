#pragma once

#include "Router.hpp"

namespace assfire
{
	class RouterProxy
	{
	public:
		virtual ~RouterProxy() {}
		virtual routing::proto::RouteInfo getRoute(routing::proto::SingleRouteRequest, const Router&, long) const = 0;
		virtual void getRoutesBatch(routing::proto::ManyToManyRoutesRequest, std::function<void(routing::proto::RouteInfo)>, const Router&, long) const = 0;
	};
}