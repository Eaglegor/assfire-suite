#include "MockProtobufClient.hpp"

using namespace assfire::router;

ProtobufClient::GetSingleRouteResponse MockProtobufClient::getRoute(const ProtobufClient::GetSingleRouteRequest &request) const {
    ProtobufClient::GetSingleRouteResponse result;
    std::string key = request.SerializeAsString();
    if(!cached_single_responses.contains(key)) {
        throw std::runtime_error("Mocked response not found");
    }
    result.ParseFromString(cached_single_responses.at(key));
    return result;
}

void MockProtobufClient::getRoutesBatch(const ProtobufClient::GetRoutesBatchRequest &request, const ProtobufClient::RoutesBatchConsumer &consumer) const {
    ProtobufClient::GetRoutesBatchResponse result;
    std::string key = request.SerializeAsString();
    if(!cached_batch_responses.contains(key)) {
        throw std::runtime_error("Mocked response not found");
    }
    result.ParseFromString(cached_batch_responses.at(key));
    consumer(result);
}

void MockProtobufClient::getRoutesBatch(const ProtobufClient::RequestSupplier &supplier, const ProtobufClient::RoutesBatchConsumer &consumer) const {
    ProtobufClient::GetRoutesBatchRequest request;
    while(supplier(request)) {
        getRoutesBatch(request, consumer);
    }
}

void MockProtobufClient::addResponse(const ProtobufClient::GetSingleRouteRequest &request, const ProtobufClient::GetSingleRouteResponse &response) {
    cached_single_responses.insert_or_assign(request.SerializeAsString(), response.SerializeAsString());
}

void MockProtobufClient::addResponse(const ProtobufClient::GetRoutesBatchRequest &request, const ProtobufClient::GetRoutesBatchResponse &response)
{
    cached_batch_responses.insert_or_assign(request.SerializeAsString(), response.SerializeAsString());
}
