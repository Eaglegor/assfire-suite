#pragma once

#include "RouteProviderProxy.hpp"

namespace assfire::router
{
	class TransparentRouteProviderProxy : public CachingRouteProviderProxy
	{
	public:
        GetSingleRouteResponse getRoute(GetSingleRouteRequest request, const Router& backend, long request_id) const
        {
            return backend.getRoute(request, request_id);
        }

        GetRoutesBatchResponse getRoutesBatch(GetRoutesBatchRequest request, const Router& backend, long request_id) const
        {
            return backend.getRoutesBatch(request, request_id);
        }

        void getRoutesBatch(GetRoutesBatchRequest request, Router::RoutesBatchConsumer consumer, const Router& backend, long request_id) const
        {
            backend.getRoutesBatch(request, consumer, request_id);
        }
	};
}