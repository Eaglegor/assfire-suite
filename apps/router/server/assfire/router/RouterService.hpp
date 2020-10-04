#pragma once

#include <assfire/api/v1/service/router/service.grpc.pb.h>
#include <assfire/router/MetricsCollector.hpp>
#include <memory>
#include <atomic>

namespace assfire::router
{
    class RouteProvider;

    class RouterService final : public assfire::api::v1::services::router::RouterService::Service
    {
    public:
        using GetSingleRouteRequest = assfire::api::v1::services::router::GetSingleRouteRequest;
        using GetSingleRouteResponse = assfire::api::v1::services::router::GetSingleRouteResponse;
        using GetRoutesBatchRequest = assfire::api::v1::services::router::GetRoutesBatchRequest;
        using GetRoutesBatchResponse = assfire::api::v1::services::router::GetRoutesBatchResponse;
        using ResponseStatus = assfire::api::v1::services::router::ResponseStatus;

        struct Options {
            MetricsCollector metrics_collector;

            bool use_redis = false;
            std::string redis_host;
            std::size_t redis_port;

            std::string osrm_endpoint = "http://router.project-osrm.org";
        };

        RouterService(const Options&);
        ~RouterService();

        grpc::Status GetSingleRoute(grpc::ServerContext *, const GetSingleRouteRequest *, GetSingleRouteResponse *) override;

        grpc::Status GetRoutesBatch(grpc::ServerContext *, const GetRoutesBatchRequest *, grpc::ServerWriter<GetRoutesBatchResponse> *) override;

        grpc::Status GetStreamingRoutesBatch(grpc::ServerContext *, grpc::ServerReaderWriter<GetRoutesBatchResponse, GetRoutesBatchRequest> *) override;

    private:
        std::unique_ptr<RouteProvider> route_provider;
        std::atomic_long request_id_counter;
        MetricsCollector metrics_collector;
    };
}