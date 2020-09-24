#pragma once

#include "RouterProxy.hpp"

namespace assfire
{
	class TransparentProxy : public RouterProxy
	{
	public:
        routing::proto::v1::GetSingleRouteResponse getRoute(routing::proto::v1::GetSingleRouteRequest request, const Router& backend, long request_id) const
        {
            return backend.getRoute(request, request_id);
        }

        routing::proto::v1::GetRoutesBatchResponse getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest request, const Router& backend, long request_id) const
        {
            return backend.getRoutesBatch(request, request_id);
        }

        void getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest request, Router::RoutesBatchConsumer consumer, const Router& backend, long request_id) const
        {
            backend.getRoutesBatch(request, consumer, request_id);
        }
	};
}