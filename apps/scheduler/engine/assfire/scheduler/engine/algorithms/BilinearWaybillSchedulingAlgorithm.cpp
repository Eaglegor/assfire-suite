#include "BilinearWaybillSchedulingAlgorithm.hpp"
#include "assfire/scheduler/api/Waybill.hpp"

using namespace assfire::scheduler;
using namespace assfire::router;

BilinearWaybillSchedulingAlgorithm::BilinearWaybillSchedulingAlgorithm(const DistanceMatrix &distance_matrix) :
        distance_matrix(distance_matrix) {}

void BilinearWaybillSchedulingAlgorithm::scheduleWaybill(Waybill &waybill) const {

    TimePoint current_time = waybill.getEarliestPossibleStartTime();
    TimeInterval shift_budget = TimeInterval::infinity();
    for (int i = 0; i < waybill.size(); ++i) {
        WaybillAllocation &allocation = waybill[i];
        if(i < waybill.size() - 1) allocation.setNextRouteInfo(distance_matrix.getRouteInfo(allocation.getLocation(), waybill[i + 1].getLocation()));
        else allocation.setNextRouteInfo(RouteInfo::zero());
        TimeWindow tw = allocation.getNearestNextTimeWindow(current_time);
        TimeInterval waiting_time = tw.getStartTime() - current_time;
        TimeInterval shift = std::min(waiting_time, shift_budget);
        if (shift.isPositive()) {
            for (int j = i; j >= 0; --j) {
                waybill[j].shiftStartTime(shift);
            }
        }
        current_time += shift;
        shift_budget -= shift;

        allocation.setStartTime(std::max(current_time, tw.getStartTime()));
        allocation.setEndTime(allocation.getStartTime() + allocation.getPlannedDuration());
        shift_budget = std::min(shift_budget, tw.getEndTime() - allocation.getEndTime());
        current_time = allocation.getEndTime() + allocation.getNextRouteInfo().getDuration();
    }
}
