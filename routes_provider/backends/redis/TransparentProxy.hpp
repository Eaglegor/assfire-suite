#pragma once

#include "RouterProxy.hpp"

namespace assfire
{
	class TransparentProxy : public RouterProxy
	{
	public:
		virtual routing::proto::RouteInfo getRoute(routing::proto::SingleRouteRequest request, const Router& backend, long request_id) const override
		{
			return backend.getRoute(request, request_id);
		}

		virtual void getRoutesBatch(routing::proto::ManyToManyRoutesRequest request, std::function<void(routing::proto::RouteInfo)> callback, const Router& backend, long request_id) const override
		{
			backend.getRoutesBatch(request, callback, request_id);
		}
	};
}