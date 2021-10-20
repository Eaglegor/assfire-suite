#include "TspService.hpp"

namespace assfire::tsp {
    TspService::TspService(std::unique_ptr<WorkerTransport> worker_task_publisher,
                           std::unique_ptr<WorkerSolutionStorage> worker_solution_storage,
                           std::unique_ptr<TaskIdGenerator> task_id_generator)
            : worker_transport(std::move(worker_task_publisher)),
              worker_solution_storage(std::move(worker_solution_storage)),
              task_id_generator(std::move(task_id_generator)) {}

    grpc::Status TspService::StartTsp(TspService::ServerContext *context, const TspService::StartTspRequest *request, TspService::StartTspResponse *response) {
        std::string task_id = task_id_generator->newId();

        WorkerTransport::WorkerTask task;
        task.set_task_id(task_id);
        task.mutable_task()->CopyFrom(request->task());

        worker_transport->publishNewTask(task);

        response->set_task_id(task_id);

        return grpc::Status::OK;
    }

    grpc::Status TspService::PauseTsp(TspService::ServerContext *context, const TspService::PauseTspRequest *request, TspService::PauseTspResponse *response) {
        worker_transport->publishPauseEvent(request->task_id());

        std::optional<WorkerSolutionStorage::Solution> solution = worker_solution_storage->fetchSolution(request->task_id());

        if (solution) {
            response->mutable_solution()->CopyFrom(*solution);
        }

        return grpc::Status::OK;
    }

    grpc::Status TspService::ResumeTsp(ServerContext *context, const ResumeTspRequest *request, ResumeTspResponse *response) {
        worker_transport->publishResumeEvent(request->task_id());

        return grpc::Status::OK;
    }

    grpc::Status TspService::StopTsp(TspService::ServerContext *context, const TspService::StopTspRequest *request, TspService::StopTspResponse *response) {
        worker_transport->publishStopEvent(request->task_id());

        std::optional<WorkerSolutionStorage::Solution> solution = worker_solution_storage->fetchSolution(request->task_id());

        if (solution) {
            response->mutable_solution()->CopyFrom(*solution);
        }

        return grpc::Status::OK;
    }

    grpc::Status TspService::GetLatestSolution(TspService::ServerContext *context, const TspService::GetLatestSolutionRequest *request, TspService::GetLatestSolutionResponse *response) {
        std::optional<WorkerSolutionStorage::Solution> solution = worker_solution_storage->fetchSolution(request->task_id());

        if (solution) {
            response->mutable_solution()->CopyFrom(*solution);
        }

        return grpc::Status::OK;
    }

    grpc::Status TspService::SubscribeForStatusUpdates(ServerContext *context, const SubscribeForStatusUpdatesRequest *request,
                                                       ServerWriter<SubscribeForStatusUpdatesResponse> *writer) {
        using namespace std::chrono_literals;

        std::unique_ptr<WorkerSolutionStorage::SolutionUpdatePublisher> solution_publisher = worker_solution_storage->subscribeForSolutionUpdate(request->task_id());

        std::atomic_bool done = false;
        std::condition_variable waiting_for_update;
        std::mutex lock;

        solution_publisher->listen([&](const WorkerSolutionStorage::Solution &solution) {
            SubscribeForStatusUpdatesResponse response;
            response.mutable_status_update()->mutable_current_best_cost()->CopyFrom(solution.cost());
            response.mutable_status_update()->mutable_current_best_validation_result()->CopyFrom(solution.validation_result());
            writer->Write(response);
            waiting_for_update.notify_all();
            done = solution.is_final();
        });

        done.wait(false);

        return grpc::Status::OK;
    }
}