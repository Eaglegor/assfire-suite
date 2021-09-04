#pragma once

#include <functional>

namespace assfire::tsp {
    class TspInterruptor {
    public:
        using InterruptionListener = std::function<void()>;

        virtual bool isInterrupted() const = 0;
        virtual void interrupt() = 0;
        virtual void setInterruptionListener(InterruptionListener interruption_listener) = 0;
    };
}