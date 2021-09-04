#include "NopTspInterruptor.hpp"

namespace assfire::tsp {

    bool NopTspInterruptor::isInterrupted() const
    {
        return false;
    }

    void NopTspInterruptor::interrupt()
    {
        // NOP
    }

    void NopTspInterruptor::setInterruptionListener(TspInterruptor::InterruptionListener interruption_listener)
    {
        // NOP
    }
}