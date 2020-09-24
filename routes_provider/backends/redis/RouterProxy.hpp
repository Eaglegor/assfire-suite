#pragma once

#include "Router.hpp"

namespace assfire
{
	class RouterProxy
	{
	public:
		virtual ~RouterProxy() {}

        virtual routing::proto::v1::GetSingleRouteResponse getRoute(routing::proto::v1::GetSingleRouteRequest, const Router& backend, long) const = 0;
        virtual routing::proto::v1::GetRoutesBatchResponse getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest, const Router& backend, long) const = 0;
        virtual void getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest, Router::RoutesBatchConsumer, const Router& backend, long) const = 0;
	};
}