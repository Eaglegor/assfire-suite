#pragma once

#include <assfire/api/v1/router/service.grpc.pb.h>
#include <assfire/router/engine/RouterEngine.hpp>
#include <memory>
#include <atomic>
#include <functional>

namespace assfire::router
{
    class RouterService final : public assfire::api::v1::router::RouterService::Service
    {
    public:
        using GetSingleRouteRequest = assfire::api::v1::router::GetSingleRouteRequest;
        using GetSingleRouteResponse = assfire::api::v1::router::GetSingleRouteResponse;
        using GetRoutesBatchRequest = assfire::api::v1::router::GetRoutesBatchRequest;
        using GetRoutesBatchResponse = assfire::api::v1::router::GetRoutesBatchResponse;
        using GetRoutesVectorRequest = assfire::api::v1::router::GetRoutesVectorRequest;
        using GetRoutesVectorResponse = assfire::api::v1::router::GetRoutesVectorResponse;
        using ResponseStatus = assfire::api::v1::router::ResponseStatus;

        struct Options {
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

        grpc::Status
        GetRoutesVector(grpc::ServerContext *context, const GetRoutesVectorRequest *request, GetRoutesVectorResponse *response) override;

    private:
        void processBatchRequest(const GetRoutesBatchRequest& request, const std::function<void(const GetRoutesBatchResponse&)> &consumeResponse);

        std::unique_ptr<RouterEngine> router_engine;
    };
}