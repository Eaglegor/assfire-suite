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
        ~TspSolutionSession();

        using Id = TspSolutionController::SessionId;

        explicit TspSolutionSession(std::unique_ptr<TspSolutionController> solution_controller);

        void interrupt();

        void pause();

        void resume();

        bool isFinished();

        void waitFor(long milliseconds);

        const Id &getId() const;

    private:
        std::unique_ptr<TspSolutionController> solution_controller;
    };
}