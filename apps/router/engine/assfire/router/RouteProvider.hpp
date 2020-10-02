#pragma once

#include <assfire/api/v1/service/router/service.pb.h>
#include <future>
#include <functional>

namespace assfire::router {
    class RouteProvider
    {
    public:
        using GetSingleRouteRequest = assfire::api::v1::services::router::GetSingleRouteRequest;
        using GetSingleRouteResponse = assfire::api::v1::services::router::GetSingleRouteResponse;
        using GetRoutesBatchRequest = assfire::api::v1::services::router::GetRoutesBatchRequest;
        using GetRoutesBatchResponse = assfire::api::v1::services::router::GetRoutesBatchResponse;
        using ResponseStatus = assfire::api::v1::services::router::ResponseStatus;

        using Location = assfire::api::v1::model::routing::Location;
        using RouteInfo = assfire::api::v1::model::routing::RouteInfo;
        using RoutingOptions = assfire::api::v1::model::routing::RoutingOptions;
        using RoutingType = RoutingOptions::RoutingType;

        using RoutesBatchConsumer = std::function<void(GetRoutesBatchResponse)>;

        virtual ~RouteProvider() = default;

        virtual GetSingleRouteResponse getRoute(GetSingleRouteRequest, long) const = 0;

        virtual GetRoutesBatchResponse getRoutesBatch(GetRoutesBatchRequest, long) const = 0;

        virtual void getRoutesBatch(GetRoutesBatchRequest, RoutesBatchConsumer, long) const = 0;

    protected:
        GetRoutesBatchResponse calculateBatchUsingSingleRoutes(GetRoutesBatchRequest request, long request_id) const
        {
            GetRoutesBatchResponse response;
            for (const Location &origin : request.origins()) {
                for (const Location &destination : request.destinations()) {
                    GetSingleRouteRequest single_request;
                    single_request.mutable_origin()->CopyFrom(origin);
                    single_request.mutable_destination()->CopyFrom(destination);
                    single_request.mutable_options()->CopyFrom(request.options());
                    response.add_route_infos()->CopyFrom(getRoute(single_request, request_id).route_info());
                }
            }
            return response;
        }
    };
}