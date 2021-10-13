#pragma once

#include "assfire/api/v1/tsp/concepts.pb.h"
#include <optional>
#include <functional>
#include <memory>

namespace assfire::tsp {
    class WorkerSolutionStorage {
    public:
        using Solution = assfire::api::v1::tsp::TspSolution;

        class SolutionUpdatePublisher {
        public:
            virtual void listen(std::function<void(const Solution&)> onSolution) = 0;
        };

        virtual std::optional<Solution> fetchSolution(const std::string& task_id) const = 0;
        virtual std::unique_ptr<SolutionUpdatePublisher> subscribeForSolutionUpdate(const std::string& task_id) = 0;
    };
}