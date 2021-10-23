#pragma once

namespace assfire::tsp {
    class TspSolution;
    class TspTask;
}
namespace assfire::tsp::worker {
    class SolutionPublisher {
    public:
        virtual ~SolutionPublisher() {
        }

        virtual void publish(const std::string &task_id, const TspTask& task, const TspSolution &solution) = 0;

        virtual void onError(const std::string &task_id) = 0;
    };
}