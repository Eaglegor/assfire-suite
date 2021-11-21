#pragma once

namespace assfire::tsp {
    class TspSolution;
    class TspTask;
}
namespace assfire::tsp {
    class SolutionPublisher {
    public:
        virtual ~SolutionPublisher() = default;

        virtual void publish(const std::string &task_id, const TspTask& task, const TspSolution &solution) = 0;
    };
}