#include <assfire/api/v1/tsp/service.grpc.pb.h>
#include <assfire/api/v1/tsp/translators/TspTaskTranslator.hpp>
#include <assfire/tsp/api/TspTask.hpp>
#include <assfire/tsp/api/TspPoint.hpp>
#include <assfire/tsp/api/TspSolverSettings.hpp>
#include <assfire/concepts/Location.hpp>
#include <grpcpp/channel.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/client_context.h>
#include <iostream>

using namespace assfire;
using namespace assfire::tsp;

std::string startTsp() {
    std::vector<TspPoint> points;
    TspPoint p1(1, Location::fromDoubleLatLon(0, 0));
    TspPoint p2(2, Location::fromDoubleLatLon(0, 1));
    TspPoint p3(3, Location::fromDoubleLatLon(0, 2));

    points.push_back(p2);
    points.push_back(p1);
    points.push_back(p3);

    TspTask task(points, TspSolverSettings());

    assfire::api::v1::tsp::StartTspRequest request;
    request.mutable_task()->CopyFrom(assfire::api::v1::tsp::TspTaskTranslator::toProto(task));

    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = assfire::api::v1::tsp::TspService::NewStub(channel);

    grpc::ClientContext context;
    assfire::api::v1::tsp::StartTspResponse response;
    stub->StartTsp(&context, request, &response);
    std::cout << "Started task " << response.task_id() << std::endl;
    return response.task_id();
}

void stopTsp(const std::string& id) {

    assfire::api::v1::tsp::StopTspRequest request;
    request.set_task_id(id);

    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = assfire::api::v1::tsp::TspService::NewStub(channel);

    grpc::ClientContext context;
    assfire::api::v1::tsp::StopTspResponse response;
    stub->StopTsp(&context, request, &response);
}

void pauseTsp(const std::string& id) {

    assfire::api::v1::tsp::PauseTspRequest request;
    request.set_task_id(id);

    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = assfire::api::v1::tsp::TspService::NewStub(channel);

    grpc::ClientContext context;
    assfire::api::v1::tsp::PauseTspResponse response;
    stub->PauseTsp(&context, request, &response);
}

void resumeTsp(const std::string& id) {

    assfire::api::v1::tsp::ResumeTspRequest request;
    request.set_task_id(id);

    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = assfire::api::v1::tsp::TspService::NewStub(channel);

    grpc::ClientContext context;
    assfire::api::v1::tsp::ResumeTspResponse response;
    stub->ResumeTsp(&context, request, &response);
}

int main() {

    startTsp();
//    stopTsp("1");
//    resumeTsp("2");
//    pauseTsp("2");

    return 0;
}