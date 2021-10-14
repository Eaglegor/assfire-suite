#pragma once

#include <functional>
#include <future>
#include "TspSolution.hpp"

namespace assfire::tsp {
    class TspSolutionController {
    public:
        virtual void interrupt() = 0;

        virtual void pause() = 0;

        virtual void resume() = 0;

        virtual std::optional<TspSolution> getCurrentSolution() = 0;

        virtual void setSolutionListener(std::function<void(const TspSolution &)> listener) = 0;

        virtual bool isFinished() = 0;
    };
}