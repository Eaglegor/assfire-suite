#pragma once

#include "assfire/tsp/TspAlgorithm.hpp"

namespace assfire::tsp
{
    class TwoOptTspAlgorithm : public TspAlgorithm
    {
    public:
        Solution solveTsp(const Task &task, const Settings &settings, const StatusConsumer &consumer, const InterruptCondition&) override;

    private:

    };
}