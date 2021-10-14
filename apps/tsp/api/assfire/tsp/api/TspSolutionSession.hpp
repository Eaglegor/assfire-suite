#pragma once

#include <functional>
#include <future>
#include <memory>
#include "TspSolution.hpp"
#include "TspSolutionController.hpp"

namespace assfire::tsp {
    class TspSolutionSession {
    public:
        TspSolutionSession(std::unique_ptr<TspSolutionController> solution_controller);

        void interrupt();
        void pause();
        void resume();
        std::future<TspSolution> getCurrentSolution();
        bool isFinished();

    private:
        std::unique_ptr<TspSolutionController> solution_controller;
    };
}