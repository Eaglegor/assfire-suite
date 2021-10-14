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
        virtual std::future<TspSolution> getCurrentSolution() = 0;
        virtual bool isFinished() = 0;
    };
}