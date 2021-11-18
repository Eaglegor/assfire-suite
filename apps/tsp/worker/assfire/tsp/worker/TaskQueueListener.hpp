#pragma once

#include <string>

namespace assfire::tsp {
    class TaskQueueListener {
    public:
        virtual ~TaskQueueListener();

        virtual void startListening() = 0;
        virtual std::string nextTask() = 0;
        virtual void ackTask(std::string& task_id) = 0;
    };
}