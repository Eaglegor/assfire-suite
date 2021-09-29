#pragma once

#include <functional>
#include "TspSolution.hpp"
#include "TspInterruptor.hpp"
#include "TspResultCallback.hpp"

namespace assfire::tsp {
    class TspTask;

    class TspSolverApi {
    public:

        virtual TspSolution solveTsp(const TspTask &task, const TspInterruptor &interruptor) const = 0;

        virtual void solveTsp(const TspTask &task, TspResultCallback resultCallback, const TspInterruptor &interruptor) const = 0;
    };
}

