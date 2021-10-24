#include "TspService.hpp"
#include <spdlog/spdlog.h>

namespace assfire::tsp {
    TspService::TspService(std::unique_ptr<WorkerTransport> worker_task_publisher,
                           std::unique_ptr<SolutionStorage> worker_solution_storage,
                           std::unique_ptr<TspTaskStorage> tsp_tasks_storage,
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

        std::optional<SolutionStorage::Solution> solution = worker_solution_storage->fetchSolution(request->task_id());

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

        std::optional<TspTaskStorage::TspTask> task = tsp_tasks_storage->fetchTask(request->task_id());

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

        std::optional<SolutionStorage::Solution> solution = worker_solution_storage->fetchSolution(request->task_id());

        if (solution) {
            response->mutable_solution()->CopyFrom(*solution);
        }

        tsp_tasks_storage->dropTask(request->task_id());

        return grpc::Status::OK;
    }

    grpc::Status TspService::GetLatestSolution(TspService::ServerContext *context, const TspService::GetLatestSolutionRequest *request, TspService::GetLatestSolutionResponse *response) {
        std::optional<SolutionStorage::Solution> solution = worker_solution_storage->fetchSolution(request->task_id());

        if (solution) {
            response->mutable_solution()->CopyFrom(*solution);
        }

        return grpc::Status::OK;
    }

    grpc::Status TspService::SubscribeForStatusUpdates(ServerContext *context, const SubscribeForStatusUpdatesRequest *request,
                                                       ServerWriter<SubscribeForStatusUpdatesResponse> *writer) {

        std::atomic_bool is_finished = false;
        worker_transport->addWorkerStatusUpdateListener(request->task_id(), [&](const WorkerTransport::WorkerStatusUpdate &update) {
            try {
                if (update.type() == WorkerTransport::WorkerStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_NEW_SOLUTION) {
                    std::optional<SolutionStorage::Solution> solution = worker_solution_storage->fetchSolution(request->task_id());
                    if (solution) {
                        SubscribeForStatusUpdatesResponse response;
                        TspStatusUpdate *status_update = response.mutable_status_update();
                        status_update->mutable_current_best_cost()->CopyFrom(solution->cost());
                        status_update->mutable_current_best_validation_result()->CopyFrom(solution->validation_result());
                        status_update->set_is_finished(solution->is_final());
                        writer->Write(response);
                        is_finished = solution->is_final();
                    }
                } else if (update.type() == WorkerTransport::WorkerStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_ERROR) {
                    SubscribeForStatusUpdatesResponse response;
                    TspStatusUpdate *status_update = response.mutable_status_update();
                    status_update->set_is_error(true);
                    writer->Write(response);
                    is_finished = true;
                }
            } catch (std::exception &e) {
                SPDLOG_ERROR("Exception while trying to retrieve solution by status update for task {}", update.task_id());
                SubscribeForStatusUpdatesResponse response;
                TspStatusUpdate *status_update = response.mutable_status_update();
                status_update->set_is_error(true);
                writer->Write(response);
                is_finished = true;
            }
        });

        try {
            std::optional<SolutionStorage::Solution> solution = worker_solution_storage->fetchSolution(request->task_id());
            if (solution) {
                SubscribeForStatusUpdatesResponse response;
                TspStatusUpdate *status_update = response.mutable_status_update();
                status_update->mutable_current_best_cost()->CopyFrom(solution->cost());
                status_update->mutable_current_best_validation_result()->CopyFrom(solution->validation_result());
                status_update->set_is_finished(solution->is_final());
                writer->Write(response);
            }
        } catch (std::exception &e) {
            SPDLOG_ERROR("Exception while trying to retrieve current solution by status update for task", request->task_id());
            SubscribeForStatusUpdatesResponse response;
            TspStatusUpdate *status_update = response.mutable_status_update();
            status_update->set_is_error(true);
            writer->Write(response);
            is_finished = true;
        }



        return grpc::Status::OK;
    }
}