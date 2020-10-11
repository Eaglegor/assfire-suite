#include "BilinearSchedulingAlgorithm.hpp"
#include <spdlog/spdlog.h>
#include <cstdint>
#include <limits>

using namespace assfire::scheduler::transport;

BilinearSchedulingAlgorithm::BilinearSchedulingAlgorithm(const RouterClient &router_client)
        : BaseSchedulingAlgorithm(router_client)
{}

void BilinearSchedulingAlgorithm::schedule(const SchedulingAlgorithm::SchedulerTask &task, SchedulingAlgorithm::SchedulerResult &result)
{
    TimePoint current_time = 0;
    Duration shift_budget = std::numeric_limits<Duration>::max();
    for (int i = 0; i < result.schedule().allocations_size(); ++i) {
        Allocation &allocation = result.mutable_schedule()->mutable_allocations()->at(i);
        const Job &job = task.jobs().at(i);
        TimeWindow &allocated_time = *allocation.mutable_allocated_time();
        Duration waiting_time = job.time_windows().at(0).start_time() - current_time;
        Duration shift = std::min(waiting_time, shift_budget);
        if (shift > 0) {
            for (int j = i; j >= 0; --j) {
                Allocation &shifted_allocation = result.mutable_schedule()->mutable_allocations()->at(i);
                shifted_allocation.mutable_allocated_time()->set_start_time(shifted_allocation.mutable_allocated_time()->start_time() + shift);
                shifted_allocation.mutable_allocated_time()->set_end_time(shifted_allocation.mutable_allocated_time()->end_time() + shift);
            }
        }
        current_time += shift;
        shift_budget -= shift;
        allocated_time.set_start_time(std::max(current_time, job.time_windows().at(0).start_time()));
        allocated_time.set_end_time(allocated_time.start_time());
        shift_budget = std::min(shift_budget, job.time_windows().at(0).end_time() - allocated_time.end_time());
        current_time = allocated_time.end_time() + allocation.route_to_next().duration();
    }
}
