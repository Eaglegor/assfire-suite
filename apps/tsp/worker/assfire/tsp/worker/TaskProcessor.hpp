#pragma once

namespace assfire::tsp::worker {
    class TaskProcessor {
    public:
        virtual void startProcessing() = 0;

        virtual ~TaskProcessor() {};
    };
}