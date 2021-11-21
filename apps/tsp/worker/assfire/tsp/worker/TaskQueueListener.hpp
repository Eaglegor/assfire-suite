#pragma once

#include <string>

namespace assfire::tsp {
    class TaskQueueListener {
    public:
        virtual ~TaskQueueListener() = default;

        virtual void startListening() = 0;
        virtual std::string nextTask() = 0;
    };
}