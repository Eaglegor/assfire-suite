#include "BilinearSchedulingAlgorithm.hpp"
#include <spdlog/spdlog.h>
#include <cstdint>

using namespace assfire::scheduler::transport;

BilinearSchedulingAlgorithm::BilinearSchedulingAlgorithm(const RouterClient &router_client)
        : BaseSchedulingAlgorithm(router_client)
{}

void BilinearSchedulingAlgorithm::schedule(const SchedulingAlgorithm::SchedulerTask &task, SchedulingAlgorithm::SchedulerResult &result)
{

}
