#include "TspService.hpp"

namespace assfire::tsp {
    TspService::TspService(std::unique_ptr<WorkerTransport> worker_task_publisher,
                           std::unique_ptr<WorkerSolutionStorage> worker_solution_storage,
                           std::unique_ptr<TspTasksStorage> tsp_tasks_storage,
                           std::unique_ptr<TaskIdGenerator> task_id_generator)
            : worker_transport(std::move(worker_task_publisher)),
              worker_solution_storage(std::move(worker_solution_storage)),
              tsp_tasks_storage(std::move(tsp_tasks_storage)),
              task_id_generator(std::move(task_id_generator)) {}

    grpc::Status TspService::StartTsp(TspService::ServerContext *context, const TspService::StartTspRequest *request, TspService::StartTspResponse *response) {
        std::string task_id = task_id_generator->newId();

        WorkerTransport::WorkerTask task;
        task.set_task_id(task_id);
        task.mutable_task()->CopyFrom(request->task());

        tsp_tasks_storage->saveTask(task.task_id(), task.task());
        tsp_tasks_storage->markAsInProgress(task.task_id());

        worker_transport->publishNewTask(task);

        response->set_task_id(task_id);

        return grpc::Status::OK;
    }

    grpc::Status TspService::PauseTsp(TspService::ServerContext *context, const TspService::PauseTspRequest *request, TspService::PauseTspResponse *response) {
        WorkerTransport::WorkerControlSignal control_signal;
        control_signal.set_task_id(request->task_id());
        control_signal.set_signal_type(WorkerTransport::WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_PAUSE);
        worker_transport->publishControlSignal(control_signal);

        std::optional<WorkerSolutionStorage::Solution> solution = worker_solution_storage->fetchSolution(request->task_id());

        if (solution) {
            response->mutable_solution()->CopyFrom(*solution);
        }
        tsp_tasks_storage->markAsStopped(request->task_id());

        return grpc::Status::OK;
    }

    grpc::Status TspService::ResumeTsp(ServerContext *context, const ResumeTspRequest *request, ResumeTspResponse *response) {
        if (tsp_tasks_storage->isInProgress(request->task_id())) {
            return grpc::Status(grpc::ALREADY_EXISTS, "Task with id = " + request->task_id() + " is already running or scheduled to be run");
        }

        std::optional<TspTasksStorage::TspTask> task = tsp_tasks_storage->fetchTask(request->task_id());

        if (task) {
            WorkerTransport::WorkerTask worker_task;
            worker_task.set_task_id(request->task_id());
            worker_task.mutable_task()->CopyFrom(*task);

            tsp_tasks_storage->markAsInProgress(request->task_id());
            worker_transport->publishNewTask(worker_task);
            return grpc::Status::OK;
        } else {
            return grpc::Status(grpc::NOT_FOUND, "No task with id = " + request->task_id() + " found");
        }
    }

    grpc::Status TspService::StopTsp(TspService::ServerContext *context, const TspService::StopTspRequest *request, TspService::StopTspResponse *response) {
        WorkerTransport::WorkerControlSignal control_signal;
        control_signal.set_task_id(request->task_id());
        control_signal.set_signal_type(WorkerTransport::WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_INTERRUPT);
        worker_transport->publishControlSignal(control_signal);

        std::optional<WorkerSolutionStorage::Solution> solution = worker_solution_storage->fetchSolution(request->task_id());

        if (solution) {
            response->mutable_solution()->CopyFrom(*solution);
        }

        tsp_tasks_storage->dropTask(request->task_id());

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