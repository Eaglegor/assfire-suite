#include "LinearSchedulingAlgorithm.hpp"
#include <spdlog/spdlog.h>
#include <cstdint>

using namespace assfire::scheduler::transport;

LinearSchedulingAlgorithm::LinearSchedulingAlgorithm(const RouterClient &router_client)
        : BaseSchedulingAlgorithm(router_client)
{}

void LinearSchedulingAlgorithm::schedule(const SchedulingAlgorithm::SchedulerTask &task, SchedulingAlgorithm::SchedulerResult &result)
{

}
