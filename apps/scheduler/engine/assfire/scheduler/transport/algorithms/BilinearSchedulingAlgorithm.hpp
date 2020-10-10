#pragma once

#include "BaseSchedulingAlgorithm.hpp"

namespace assfire::scheduler::transport
{
    class BilinearSchedulingAlgorithm : public BaseSchedulingAlgorithm
    {
    public:
        BilinearSchedulingAlgorithm(const RouterClient &router_client);

    private:
        void schedule(const SchedulerTask &task, SchedulerResult& result) override;
    };
}