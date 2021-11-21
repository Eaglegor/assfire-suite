#pragma once

#include <vector>
#include "WaybillAllocation.hpp"

namespace assfire::scheduler {
    class Waybill {
    public:
        using size_t = int;
        using Allocations = std::vector<WaybillAllocation>;

        Waybill(const TimeWindow &work_shift, Allocations allocations);
        Waybill(const TimeWindow &work_shift);

        Waybill& operator=(const Waybill& rhs) = default;

        TimePoint getEarliestPossibleStartTime() const;

        TimePoint getLatestPossibleEndTime() const;

        const Allocations& getAllocations() const;

        const TimeWindow& getWorkShift() const;
        WaybillAllocation& operator[](size_t i);
        const WaybillAllocation& operator[](size_t i) const;

        size_t size() const;
        void push_back(const WaybillAllocation& waybill_allocation);

        void clear();

    private:
        TimeWindow work_shift;
        Allocations allocations;
    };
}