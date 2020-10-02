#include "GreeterService.h"

assfire::GreeterService::GreeterService(const assfire::MetricsCollector &metrics_collector) :
        greeter(metrics_collector)
{
}

grpc::Status assfire::GreeterService::Greet(::grpc::ServerContext *context, const ::assfire::api::v1::services::sample::GreetRequest *request, ::assfire::api::v1::services::sample::GreetResponse *response)
{
    response->mutable_greeting()->assign(greeter.greet(request->user().username()));
    return grpc::Status::OK;
}

assfire::GreeterService::~GreeterService() = default;
