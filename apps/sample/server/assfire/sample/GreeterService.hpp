#pragma once

#include <assfire/api/v1/service/sample/sample.grpc.pb.h>
#include <assfire/sample/MetricsCollector.hpp>
#include <assfire/sample/Greeter.hpp>

namespace assfire::sample {
    class GreeterService : public api::v1::service::sample::GreeterService::Service
    {
    public:
        explicit GreeterService(const MetricsCollector &metrics_collector);

        virtual ~GreeterService();

        grpc::Status Greet(::grpc::ServerContext *context, const ::assfire::api::v1::service::sample::GreetRequest *request, ::assfire::api::v1::service::sample::GreetResponse *response) override;

    private:
        Greeter greeter;
    };
}
