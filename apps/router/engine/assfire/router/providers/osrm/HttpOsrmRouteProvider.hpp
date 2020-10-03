#pragma once

#include "assfire/router/MetricsCollector.hpp"
#include "assfire/router/RouteProvider.hpp"

namespace web::http::client {
    class http_client;
}

namespace assfire::router {
    class HttpOsrmRouteProvider : public RouteProvider
    {
    public:
        using OsrmOptions = assfire::api::v1::model::routing::RoutingOptions::Osrm;

        HttpOsrmRouteProvider(const std::string &osrm_endpoint, const MetricsCollector &metrics_context = MetricsCollector());

        virtual ~HttpOsrmRouteProvider();

        GetSingleRouteResponse getRoute(GetSingleRouteRequest, long) const override;

        GetRoutesBatchResponse getRoutesBatch(GetRoutesBatchRequest, long) const override;

        void getRoutesBatch(GetRoutesBatchRequest, RoutesBatchConsumer, long) const override;

    private:
        GetRoutesBatchResponse calculateRoutesBatch(GetRoutesBatchRequest request, long request_id) const;

        MetricsCollector metrics_collector;
        std::unique_ptr<web::http::client::http_client> client;
    };
}

