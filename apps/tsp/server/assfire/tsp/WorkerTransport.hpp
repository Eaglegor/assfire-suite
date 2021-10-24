#pragma once

#include "assfire/api/v1/tsp/worker.pb.h"

namespace assfire::tsp {
    class WorkerTransport {
    public:
        using WorkerTask = assfire::api::v1::tsp::WorkerTask;
        using WorkerControlSignal = assfire::api::v1::tsp::WorkerControlSignal;

        virtual void publishNewTask(const WorkerTask &task) = 0;
        virtual void publishControlSignal(const WorkerControlSignal &signal) = 0;

        virtual ~WorkerTransport() {}
    };
}