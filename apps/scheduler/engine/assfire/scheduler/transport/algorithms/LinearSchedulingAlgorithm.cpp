#include "LinearSchedulingAlgorithm.hpp"
#include <spdlog/spdlog.h>
#include <cstdint>
#include <algorithm>

using namespace assfire::scheduler::transport;

LinearSchedulingAlgorithm::LinearSchedulingAlgorithm(const RouterClient &router_client)
        : BaseSchedulingAlgorithm(router_client)
{}

void LinearSchedulingAlgorithm::schedule(const SchedulingAlgorithm::SchedulerTask &task, SchedulingAlgorithm::SchedulerResult &result)
{
    TimePoint current_time = 0;
    for(int i = 0; i < result.schedule().allocations_size(); ++i) {
        Allocation& allocation = result.mutable_schedule()->mutable_allocations()->at(i);
        const Job& job = task.jobs().at(i);
        TimeWindow& allocated_time = *allocation.mutable_allocated_time();
        allocated_time.set_start_time(std::max(current_time, job.time_windows().at(0).start_time()));
        allocated_time.set_end_time(allocated_time.start_time());
        current_time = allocated_time.end_time() + allocation.route_to_next().duration();
    }
}
