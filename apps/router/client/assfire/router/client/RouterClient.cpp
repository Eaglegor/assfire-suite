#include "RouterClient.hpp"
#include "assfire/router/client/transport/GrpcClient.hpp"
#include "assfire/router/client/strategies/DirectRequestStrategy.hpp"
#include <spdlog/spdlog.h>

assfire::router::RouterClient::RouterClient(const std::string &server_host, Port server_port, bool use_ssl) :
        transport_client(std::make_unique<GrpcClient>(server_host, server_port, use_ssl))
{
}

assfire::router::RouterClient::~RouterClient() = default;

assfire::router::DistanceMatrix assfire::router::RouterClient::createDistanceMatrix(assfire::router::RouterClient::RequestStrategy strategy)
{
    switch(strategy) {
        case RequestStrategy::DIRECT_REQUEST:
            return DistanceMatrix(std::make_unique<assfire::router::DirectRequestStrategy>(*transport_client));
        default:
            SPDLOG_ERROR("Unknown request strategy for distance matrix: {}", strategy);
            throw std::invalid_argument("Unknown request strategy for distance matrix");
    }
}
