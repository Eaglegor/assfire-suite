#pragma once
#include <concepts>
#include <assfire/router/api/model/Location.hpp>
#include <assfire/router/api/DistanceMatrix.hpp>

namespace assfire::scheduler {
    template<typename T, typename TimePoint = decltype(&T::getAllocatedTime()), typename TimeInterval = decltype(TimePoint() - TimePoint())>
    concept WaybillSchedulable = requires(T a) {
        {a.getAllocatedTime()} -> std::convertible_to<TimePoint>;
        {a.getAllocatedTime() - a.getAllocatedTime()} -> std::convertible_to<TimeInterval>;
        {a.setAllocatedTime(TimePoint())} -> std::same_as<void>;
        {a.getLocation()} -> std::convertible_to<const router::Location&>;
        {a.getDistanceMatrix()} -> std::convertible_to<const router::DistanceMatrix&>;
    };
}

