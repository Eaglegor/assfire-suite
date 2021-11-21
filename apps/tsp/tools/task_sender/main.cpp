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

#include <cxxopts.hpp>

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
    request.mutable_task()->mutable_solver_settings()->mutable_algorithm_settings()->set_algorithm_type(assfire::api::v1::tsp::TSP_ALGORITHM_TYPE_TIME_WASTING);

    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = assfire::api::v1::tsp::TspService::NewStub(channel);

    grpc::ClientContext context;
    assfire::api::v1::tsp::StartTspResponse response;
    ::grpc::Status status = stub->StartTsp(&context, request, &response);
    if (!status.ok()) {
        std::cout << "Failed to start task: " << status.error_message() << std::endl;
    } else {
        std::cout << "Started task " << response.task_id() << std::endl;
    }
    return response.task_id();
}

void listenToUpdates(const std::string &task) {
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = assfire::api::v1::tsp::TspService::NewStub(channel);

    assfire::api::v1::tsp::SubscribeForStatusUpdatesRequest status_request;
    status_request.set_task_id(task);
    grpc::ClientContext context2;
    std::unique_ptr<::grpc::ClientReader<assfire::api::v1::tsp::SubscribeForStatusUpdatesResponse>> reader(stub->SubscribeForStatusUpdates(&context2, status_request));
    assfire::api::v1::tsp::SubscribeForStatusUpdatesResponse status_response;
    while (reader->Read(&status_response)) {
        std::cout << "Got status update for task: " << task << " :: " << assfire::api::v1::tsp::TspStatusUpdate_Type_Name(status_response.status_update().type()) << std::endl;
        if (status_response.status_update().type() == assfire::api::v1::tsp::TspStatusUpdate_Type_TSP_STATUS_UPDATE_TYPE_NEW_SOLUTION) {
            std::cout << "Solution cost: " << status_response.status_update().new_solution_cost().value() << std::endl;
        }
    }
    reader->Finish();
}

void stopTsp(const std::string &id) {

    assfire::api::v1::tsp::StopTspRequest request;
    request.set_task_id(id);

    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = assfire::api::v1::tsp::TspService::NewStub(channel);

    grpc::ClientContext context;
    assfire::api::v1::tsp::StopTspResponse response;
    ::grpc::Status status = stub->StopTsp(&context, request, &response);

    if (!status.ok()) {
        std::cout << "Failed to stop task: " << status.error_message() << std::endl;
    } else {
        std::cout << "Stopped task " << id << std::endl;
    }
}

void pauseTsp(const std::string &id) {

    assfire::api::v1::tsp::PauseTspRequest request;
    request.set_task_id(id);

    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = assfire::api::v1::tsp::TspService::NewStub(channel);

    grpc::ClientContext context;
    assfire::api::v1::tsp::PauseTspResponse response;
    ::grpc::Status status = stub->PauseTsp(&context, request, &response);
    if (!status.ok()) {
        std::cout << "Failed to pause task: " << status.error_message() << std::endl;
    } else {
        std::cout << "Paused task " << id << std::endl;
    }
}

void resumeTsp(const std::string &id) {

    assfire::api::v1::tsp::ResumeTspRequest request;
    request.set_task_id(id);

    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = assfire::api::v1::tsp::TspService::NewStub(channel);

    grpc::ClientContext context;
    assfire::api::v1::tsp::ResumeTspResponse response;
    ::grpc::Status status = stub->ResumeTsp(&context, request, &response);
    if (!status.ok()) {
        std::cout << "Failed to resume task: " << status.error_message() << std::endl;
    } else {
        std::cout << "Resumed task " << id << std::endl;
    }
}

void getSolution(const std::string &id) {

    assfire::api::v1::tsp::GetLatestSolutionRequest request;
    request.set_task_id(id);

    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = assfire::api::v1::tsp::TspService::NewStub(channel);

    grpc::ClientContext context;
    assfire::api::v1::tsp::GetLatestSolutionResponse response;
    ::grpc::Status status = stub->GetLatestSolution(&context, request, &response);
    if (!status.ok()) {
        std::cout << "Failed to retrieve solution task: " << status.error_message() << std::endl;
    } else {
        std::cout << "Solution:  " << id << std::endl;
        auto sol = response.solution();
        for(int i = 0; i < sol.optimized_sequence_size(); ++i) {
            std::cout << (i == 0 ? "" : " - ") << sol.optimized_sequence(i);
        }
        std::cout << std::endl;
    }
}

constexpr const char *ACTION = "action";
constexpr const char *TASK_ID = "task-id";

int main(int argc, char *argv[]) {

    cxxopts::Options args_template("assfire-tsp-task-sender");

    args_template
            .custom_help("")
            .positional_help("action [task_id]")
            .show_positional_help();

    args_template.add_options()
    (ACTION, "Action to take (start/stop/pause/resume/listen)", cxxopts::value<std::string>())
    (TASK_ID, "Task id to apply action to", cxxopts::value<std::string>()->default_value("<none>"));

    args_template.parse_positional({ACTION, TASK_ID});

    auto options = args_template.parse(argc, argv);

    if(!options.count(ACTION)) {
        std::cout << args_template.help() << std::endl;
        exit(0);
    }

    std::string action = options[ACTION].as<std::string>();
    std::string task = options.count(TASK_ID) > 0 ? options[TASK_ID].as<std::string>() : "";

    if (action == "start") {
        startTsp();
    } else if (action == "stop") {
        stopTsp(task);
    } else if (action == "pause") {
        pauseTsp(task);
    } else if (action == "resume") {
        resumeTsp(task);
    } else if (action == "status" || action == "listen") {
        listenToUpdates(task);
    } else if (action == "solution") {
        getSolution(task);
    }

    return 0;
}