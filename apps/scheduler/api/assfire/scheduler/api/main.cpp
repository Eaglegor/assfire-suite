#include "model/WaybillSchedulable.hpp"
#include "model/WaybillRepresentable.hpp"

using namespace assfire::scheduler;

class AllocationImpl
{
//    {a.getAllocatedTime()} -> std::convertible_to<TimePoint>;
//    {a.getAllocatedTime() - a.getAllocatedTime()} -> std::convertible_to<TimeInterval>;
//    {a.setAllocatedTime(TimePoint())} -> std::same_as<void>;
//    {a.getLocation()} -> std::convertible_to<const router::Location&>;
//    {a.getDistanceMatrix()} -> std::convertible_to<const router::DistanceMatrix&>;
public:
    using TimePointType = long;

    TimePointType getAllocatedTime() {
        return 0;
    }

    void setAllocatedTime(TimePointType tp) {

    }

    const assfire::router::Location& getLocation() {
        return assfire::router::Location();
    }

    const assfire::router::DistanceMatrix& getDistanceMatrix() {
        return assfire::router::DistanceMatrix(nullptr);
    }
};

class ScheduleImpl {
public:
    using TimePointType = AllocationImpl::TimePointType;
    using AllocationType = AllocationImpl;

    AllocationImpl& operator[](int index) {
        return alloc;
    }

    AllocationImpl alloc;
};

template<WaybillSchedulable Allocation>
void testFunction(Allocation& t) {
    auto tp1 = t.getAllocatedTime();
    auto tp2 = t.getAllocatedTime();

    auto time_interval = tp1 - tp2;
    t.setAllocatedTime(tp2);


}

template<WaybillRepresentable Schedule>
void testScheduleFunction(Schedule& s) {
    auto& alloc = s[0];
}

void testInvoke()
{
    ScheduleImpl impl;

    testScheduleFunction(impl);
}