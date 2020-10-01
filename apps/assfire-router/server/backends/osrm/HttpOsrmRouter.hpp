#pragma once

#include <RoutingMetricsCollector.hpp>
#include "Router.hpp"

namespace web::http::client {
    class http_client;
}

namespace assfire {
    class HttpOsrmRouter : public Router
    {
    public:
        HttpOsrmRouter(const std::string &osrm_endpoint, const RoutingMetricsCollector &metrics_context = RoutingMetricsCollector());

        virtual ~HttpOsrmRouter();

        routing::proto::v1::GetSingleRouteResponse getRoute(routing::proto::v1::GetSingleRouteRequest, long) const override;

        routing::proto::v1::GetRoutesBatchResponse getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest, long) const override;

        void getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest, RoutesBatchConsumer, long) const override;

    private:
        routing::proto::v1::GetRoutesBatchResponse calculateRoutesBatch(routing::proto::v1::GetRoutesBatchRequest request, long request_id) const;

        RoutingMetricsCollector metrics_context;
        std::unique_ptr<web::http::client::http_client> client;
    };
}

