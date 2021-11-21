#include "Waybill.hpp"

#include <utility>

using namespace assfire::scheduler;

Waybill::Waybill(const TimeWindow &work_shift, Waybill::Allocations allocations)
        : work_shift(work_shift),
          allocations(std::move(allocations)) {
}

Waybill::Waybill(const TimeWindow &work_shift)
        : work_shift(work_shift) {

}

assfire::TimePoint Waybill::getEarliestPossibleStartTime() const {
    return work_shift.getStartTime();
}

assfire::TimePoint Waybill::getLatestPossibleEndTime() const {
    return work_shift.getEndTime();
}

WaybillAllocation &Waybill::operator[](Waybill::size_t i) {
    return allocations[i];
}

const WaybillAllocation &Waybill::operator[](Waybill::size_t i) const {
    return allocations[i];
}

Waybill::size_t Waybill::size() const {
    return allocations.size();
}

void Waybill::push_back(const WaybillAllocation &waybill_allocation) {
    return allocations.push_back(waybill_allocation);
}

void Waybill::clear() {
    return allocations.clear();
}

const TimeWindow &Waybill::getWorkShift() const {
    return work_shift;
}

const Waybill::Allocations &Waybill::getAllocations() const {
    return allocations;
}
