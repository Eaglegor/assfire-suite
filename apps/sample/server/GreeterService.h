#pragma once

#include <assfire/api/v1/service/sample/service.grpc.pb.h>
#include <assfire/sample/MetricsCollector.hpp>
#include <assfire/sample/Greeter.hpp>

namespace assfire {
    class GreeterService : public api::v1::services::sample::GreeterService::Service
    {
    public:
        explicit GreeterService(const MetricsCollector& metrics_collector);

        virtual ~GreeterService();

        grpc::Status Greet(::grpc::ServerContext *context, const ::assfire::api::v1::services::sample::GreetRequest *request, ::assfire::api::v1::services::sample::GreetResponse *response) override;

    private:
        assfire::Greeter greeter;
    };
}
