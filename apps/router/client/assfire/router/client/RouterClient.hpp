#pragma once

#include <memory>
#include <string>
#include "DistanceMatrix.hpp"

namespace assfire::router
{
    class GrpcClient;

    class RouterClient final
    {
    public:
        enum class RequestStrategy
        {
            UNSPECIFIED,
            DIRECT_REQUEST
        };
        using Port = std::size_t;

        RouterClient(const std::string &server_host, Port server_port, bool use_ssl);

        ~RouterClient();

        DistanceMatrix createDistanceMatrix(RequestStrategy strategy = RequestStrategy::UNSPECIFIED) const;

    private:
        std::unique_ptr<GrpcClient> transport_client;
    };
}