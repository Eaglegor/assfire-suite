#pragma once

#include "TspInterruptor.hpp"

namespace assfire::tsp
{
    class NopTspInterruptor : public TspInterruptor
    {
    public:
        bool isInterrupted() const override;

        void interrupt() override;

        void setInterruptionListener(InterruptionListener interruption_listener) override;
    };
}