#include "LinearWaybillSchedulingAlgorithm.hpp"
#include "assfire/scheduler/api/Waybill.hpp"

using namespace assfire::scheduler;

void LinearWaybillSchedulingAlgorithm::scheduleWaybill(Waybill &waybill) const {
    TimePoint current_time = waybill.getEarliestPossibleStartTime();
    for(int i = 0; i < waybill.size(); ++i) {
        WaybillAllocation& allocation = waybill[i];
        TimeWindow tw = allocation.getNearestNextTimeWindow(current_time);
        allocation.setStartTime(std::max(current_time, tw.getStartTime()));
        allocation.setEndTime(allocation.getStartTime() + allocation.getPlannedDuration());
        current_time = allocation.getStartTime() + allocation.getNextRouteInfo().getDuration();
    }
}
