#pragma once

#include "assfire/api/v1/tsp/worker.pb.h"

namespace assfire::tsp {
    class WorkerTransport {
    public:
        using WorkerTask = assfire::api::v1::tsp::WorkerTask;
        using WorkerControlSignal = assfire::api::v1::tsp::WorkerControlSignal;
        using WorkerStatusUpdate = assfire::api::v1::tsp::WorkerTspStatusUpdate;
        using StatusUpdateListener = std::function<void(const WorkerStatusUpdate &status_update)>;

        virtual void publishNewTask(const WorkerTask &task) = 0;

        virtual void publishControlSignal(const WorkerControlSignal &signal) = 0;

        virtual void addWorkerStatusUpdateListener(const std::string &task_id, StatusUpdateListener listener) = 0;

        virtual void removeStatusUpdateListener(const std::string &task_id) = 0;

        virtual ~WorkerTransport() {}
    };
}