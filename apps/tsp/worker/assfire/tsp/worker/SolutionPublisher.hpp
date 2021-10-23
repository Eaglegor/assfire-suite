#pragma once

namespace assfire::tsp {
    class TspSolution;
}
namespace assfire::tsp::worker {
    class SolutionPublisher {
    public:

        virtual void publish(const std::string& task_id, const TspSolution &solution) = 0;
    };
}