#pragma once

#include <functional>
#include <unordered_map>
#include <string>

#include <assfire/router/client/transport/ProtobufClient.hpp>

namespace assfire::router
{
    class MockProtobufClient : public ProtobufClient {
    public:
        GetSingleRouteResponse getRoute(const GetSingleRouteRequest &request) const override;

        void getRoutesBatch(const GetRoutesBatchRequest &request, const RoutesBatchConsumer &consumer) const override;

        void getRoutesBatch(const RequestSupplier &supplier, const RoutesBatchConsumer &consumer) const override;

        void addResponse(const GetSingleRouteRequest& request, const GetSingleRouteResponse& response);

    private:
        std::unordered_map<std::string, std::string> cached_single_responses;
    };
}