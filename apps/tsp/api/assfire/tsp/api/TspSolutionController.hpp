#pragma once

#include <functional>
#include <future>
#include "TspSolution.hpp"

namespace assfire::tsp {
    class TspSolutionController {
    public:
        using SessionId = std::string;

        virtual void interrupt() = 0;

        virtual void pause() = 0;

        virtual void resume() = 0;

        virtual bool isFinished() = 0;

        virtual void waitFor(long milliseconds) = 0;

        virtual const SessionId &getSessionId() const = 0;
    };
}