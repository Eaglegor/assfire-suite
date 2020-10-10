#pragma once

#include "BaseSchedulingAlgorithm.hpp"

namespace assfire::scheduler::transport
{
    class LinearSchedulingAlgorithm : public BaseSchedulingAlgorithm
    {
    public:
        LinearSchedulingAlgorithm(const RouterClient &router_client);

    private:
        void schedule(const SchedulerTask &task, SchedulerResult& result) override;
    };
}