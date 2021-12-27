#pragma once

#include <string>
#include <functional>

namespace assfire::tsp {
    class TaskQueueListener {
    public:
        using NewTaskProcessor = std::function<void(std::string)>;

        virtual ~TaskQueueListener() = default;

        virtual void startListening() = 0;
        virtual void nextTask(NewTaskProcessor process) = 0;
    };
}