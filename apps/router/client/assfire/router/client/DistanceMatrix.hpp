#pragma once
#include <memory>
#include <assfire/api/v1/service/router/router.pb.h>

namespace assfire::router
{
    class RequestStrategy;

    class DistanceMatrix
    {
    public:
        using GetSingleRouteRequest = assfire::api::v1::service::router::GetSingleRouteRequest;
        using GetSingleRouteResponse = assfire::api::v1::service::router::GetSingleRouteResponse;
        using GetRoutesBatchRequest = assfire::api::v1::service::router::GetRoutesBatchRequest;
        using GetRoutesBatchResponse = assfire::api::v1::service::router::GetRoutesBatchResponse;
        using ResponseStatus = assfire::api::v1::service::router::ResponseStatus;

        using Location = assfire::api::v1::model::routing::Location;
        using RoutingOptions = assfire::api::v1::model::routing::RoutingOptions;
        using RouteInfo = assfire::api::v1::model::routing::RouteInfo;

        using RoutesBatchConsumer = std::function<void(GetRoutesBatchResponse)>;
        using RequestSupplier = std::function<bool(GetRoutesBatchRequest&)>;

        using LocationId = int;
        using RoutingOptionsId = int;

        explicit DistanceMatrix(std::unique_ptr<RequestStrategy> &&request_strategy);
        DistanceMatrix(DistanceMatrix&&);
        ~DistanceMatrix();

        LocationId addLocation(const Location& location);
        RoutingOptionsId addRoutingOptions(const RoutingOptions& routing_options);

        void prepareRoute(const GetSingleRouteRequest &);
        void prepareRoute(const Location& origin, const Location& destination, const RoutingOptions& options);
        void prepareRoute(LocationId origin, LocationId destination, RoutingOptionsId options);
        void prepareRoutes(const GetRoutesBatchRequest &);

        [[nodiscard]]
        GetSingleRouteResponse getRoute(const GetSingleRouteRequest &) const;
        void getRoutesBatch(const GetRoutesBatchRequest &, const RoutesBatchConsumer &) const;
        void getRoutesBatch(const RequestSupplier&, const RoutesBatchConsumer &) const;

        RouteInfo getRoute(const Location& origin, const Location& destination, const RoutingOptions& options) const;
        RouteInfo getRoute(LocationId origin, LocationId destination, RoutingOptionsId options) const;

    private:
        std::unique_ptr<RequestStrategy> request_strategy;
    };
}