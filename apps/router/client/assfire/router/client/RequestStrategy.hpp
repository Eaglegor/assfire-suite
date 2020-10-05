#pragma once

#include <assfire/api/v1/service/router/service.pb.h>

namespace assfire::router
{
    class RequestStrategy {
    public:
        using GetSingleRouteRequest = assfire::api::v1::services::router::GetSingleRouteRequest;
        using GetSingleRouteResponse = assfire::api::v1::services::router::GetSingleRouteResponse;
        using GetRoutesBatchRequest = assfire::api::v1::services::router::GetRoutesBatchRequest;
        using GetRoutesBatchResponse = assfire::api::v1::services::router::GetRoutesBatchResponse;
        using ResponseStatus = assfire::api::v1::services::router::ResponseStatus;

        using Location = assfire::api::v1::model::routing::Location;
        using RoutingOptions = assfire::api::v1::model::routing::RoutingOptions;
        using RouteInfo = assfire::api::v1::model::routing::RouteInfo;

        using RoutesBatchConsumer = std::function<void(GetRoutesBatchResponse)>;
        using RequestSupplier = std::function<bool(GetRoutesBatchRequest&)>;

        using LocationId = int;
        using RoutingOptionsId = int;

        virtual LocationId addLocation(const Location& location) = 0;
        virtual RoutingOptionsId addRoutingOptions(const RoutingOptions& routing_options) = 0;

        virtual void prepareRoute(const GetSingleRouteRequest &) = 0;
        virtual void prepareRoutes(const GetRoutesBatchRequest &) = 0;
        virtual void prepareRoute(const Location& origin, const Location& destination, const RoutingOptions& options) = 0;
        virtual void prepareRoute(LocationId origin, LocationId destination, RoutingOptionsId options) = 0;

        [[nodiscard]]
        virtual GetSingleRouteResponse getRoute(const GetSingleRouteRequest &) const = 0;
        virtual void getRoutesBatch(const GetRoutesBatchRequest &, const RoutesBatchConsumer &) const = 0;
        virtual void getRoutesBatch(const RequestSupplier&, const RoutesBatchConsumer &) const = 0;

        virtual RouteInfo getRoute(const Location& origin, const Location& destination, const RoutingOptions& options) const = 0;
        virtual RouteInfo getRoute(LocationId origin, LocationId destination, RoutingOptionsId options) = 0;
    };
}