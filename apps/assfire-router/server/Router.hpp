#pragma once

#include "assfire/routing/v1/routing.pb.h"
#include <future>
#include <functional>

namespace assfire {
    class Router {
    public:
        using RoutesBatchConsumer = std::function<void(routing::proto::v1::GetRoutesBatchResponse)>;

        virtual ~Router() {}

        virtual routing::proto::v1::GetSingleRouteResponse getRoute(routing::proto::v1::GetSingleRouteRequest, long) const = 0;

        virtual routing::proto::v1::GetRoutesBatchResponse
        getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest, long) const = 0;

        virtual void getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest, RoutesBatchConsumer, long) const = 0;

    protected:
        routing::proto::v1::GetRoutesBatchResponse
        calculateBatchUsingSingleRoutes(routing::proto::v1::GetRoutesBatchRequest request, long request_id) const {
            routing::proto::v1::GetRoutesBatchResponse response;
            for (const routing::proto::v1::Location &origin : request.origins()) {
                for (const routing::proto::v1::Location &destination : request.destinations()) {
                    routing::proto::v1::GetSingleRouteRequest single_request;
                    single_request.mutable_from()->CopyFrom(origin);
                    single_request.mutable_to()->CopyFrom(destination);
                    single_request.mutable_options()->CopyFrom(request.options());
                    response.add_route_infos()->CopyFrom(getRoute(single_request, request_id).route_info());
                }
            }
            return response;
        }
    };
}