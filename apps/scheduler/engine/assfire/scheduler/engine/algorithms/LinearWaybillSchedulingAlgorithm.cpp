#include "LinearWaybillSchedulingAlgorithmTest.hpp"
#include "assfire/scheduler/api/Waybill.hpp"

using namespace assfire::router;
using namespace assfire::scheduler;

LinearWaybillSchedulingAlgorithm::LinearWaybillSchedulingAlgorithm(const assfire::router::DistanceMatrix &distance_matrix) :
        distance_matrix(distance_matrix) {}

void LinearWaybillSchedulingAlgorithm::scheduleWaybill(Waybill &waybill) const {
    TimePoint current_time = waybill.getEarliestPossibleStartTime();
    for (int i = 0; i < waybill.size(); ++i) {
        WaybillAllocation &allocation = waybill[i];
        if(i < waybill.size() - 1) allocation.setNextRouteInfo(distance_matrix.getRouteInfo(allocation.getLocation(), waybill[i + 1].getLocation()));
        else allocation.setNextRouteInfo(RouteInfo::zero());
        TimeWindow tw = allocation.getNearestNextTimeWindow(current_time);
        allocation.setStartTime(std::max(current_time, tw.getStartTime()));
        allocation.setEndTime(allocation.getStartTime() + allocation.getPlannedDuration());
        current_time = allocation.getEndTime() + allocation.getNextRouteInfo().getDuration();
    }
}
