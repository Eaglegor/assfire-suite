#pragma once

#include <functional>
#include <future>
#include <memory>
#include <chrono>
#include "TspSolution.hpp"
#include "TspSolutionController.hpp"

namespace assfire::tsp {
    class TspSolutionSession {
    public:
        TspSolutionSession(std::unique_ptr<TspSolutionController> solution_controller);

        void interrupt();

        void pause();

        void resume();

        void setSolutionListener(std::function<void(const TspSolution &)> solution_listener);

        bool isFinished();

        void waitFor(long milliseconds);

    private:
        std::unique_ptr<TspSolutionController> solution_controller;
    };
}