#include "GreeterService.hpp"

namespace assfire::sample {
    GreeterService::GreeterService(const MetricsCollector &metrics_collector) :
            greeter(metrics_collector)
    {
    }

    grpc::Status GreeterService::Greet(::grpc::ServerContext *context, const ::assfire::api::v1::service::sample::GreetRequest *request, ::assfire::api::v1::service::sample::GreetResponse *response)
    {
        response->mutable_greeting()->assign(greeter.greet(request->user().username()));
        return grpc::Status::OK;
    }

    GreeterService::~GreeterService() = default;
}