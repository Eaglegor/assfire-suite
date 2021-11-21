#pragma once

#include "assfire/tsp/api/TspCost.hpp"
#include "assfire/tsp/api/TspValidationResult.hpp"

namespace assfire::tsp {
    class StatusPublisher {
    public:
        virtual ~StatusPublisher() = default;

        virtual void release(const std::string& task_id) = 0;
        virtual void publishStarted(std::string& task_id) = 0;
        virtual void publishPaused(std::string& task_id) = 0;
        virtual void publishInterrupted(std::string& task_id) = 0;
        virtual void publishError(std::string& task_id) = 0;
        virtual void publishFinished(std::string& task_id) = 0;
        virtual void publishNewSolution(std::string& task_id, const TspCost& cost, const TspValidationResult& validation_result) = 0;
    };
}