#pragma once

namespace assfire::tsp {
    class TaskProcessor {
    public:
        virtual void startProcessing() = 0;

        virtual ~TaskProcessor() {};
    };
}