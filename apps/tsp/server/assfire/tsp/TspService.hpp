#pragma once

#include <assfire/api/v1/tsp/service.grpc.pb.h>
#include "assfire/tsp/worker/WorkerTaskPublisher.hpp"
#include "assfire/tsp/WorkerSolutionStorage.hpp"
#include "TaskIdGenerator.hpp"
#include <memory>
#include <atomic>
#include <functional>

namespace assfire::tsp {
    class TspService final : public assfire::api::v1::tsp::TspService::Service {
    public:
        using ServerContext = ::grpc::ServerContext;
        template<typename T> using ServerWriter = ::grpc::ServerWriter<T>;
        using StartTspRequest = api::v1::tsp::StartTspRequest;
        using StartTspResponse = api::v1::tsp::StartTspResponse;
        using PauseTspRequest = api::v1::tsp::PauseTspRequest;
        using PauseTspResponse = api::v1::tsp::PauseTspResponse;
        using ResumeTspRequest = api::v1::tsp::ResumeTspRequest;
        using ResumeTspResponse = api::v1::tsp::ResumeTspResponse;
        using StopTspRequest = api::v1::tsp::StopTspRequest;
        using StopTspResponse = api::v1::tsp::StopTspResponse;
        using GetLatestSolutionRequest = api::v1::tsp::GetLatestSolutionRequest;
        using GetLatestSolutionResponse = api::v1::tsp::GetLatestSolutionResponse;
        using SubscribeForStatusUpdatesRequest = api::v1::tsp::SubscribeForStatusUpdatesRequest;
        using SubscribeForStatusUpdatesResponse = api::v1::tsp::SubscribeForStatusUpdatesResponse;
        using WorkerTaskPublisher = worker::WorkerTaskPublisher;

        explicit TspService(
                std::unique_ptr<WorkerTaskPublisher> worker_task_publisher,
                std::unique_ptr<WorkerSolutionStorage> worker_solution_storage,
                std::unique_ptr<TaskIdGenerator> task_id_generator);

        grpc::Status StartTsp(ServerContext *context, const StartTspRequest *request, StartTspResponse *response) override;

        grpc::Status PauseTsp(ServerContext *context, const PauseTspRequest *request, PauseTspResponse *response) override;

        grpc::Status ResumeTsp(ServerContext *context, const ResumeTspRequest *request, ResumeTspResponse *response) override;

        grpc::Status StopTsp(ServerContext *context, const StopTspRequest *request, StopTspResponse *response) override;

        grpc::Status GetLatestSolution(ServerContext *context, const GetLatestSolutionRequest *request, GetLatestSolutionResponse *response) override;

        grpc::Status SubscribeForStatusUpdates(ServerContext *context, const SubscribeForStatusUpdatesRequest *request, ServerWriter<SubscribeForStatusUpdatesResponse> *writer) override;

    private:
        std::unique_ptr<WorkerTaskPublisher> worker_task_publisher;
        std::unique_ptr<WorkerSolutionStorage> worker_solution_storage;
        std::unique_ptr<TaskIdGenerator> task_id_generator;
    };
}