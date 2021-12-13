#pragma once

#include "assfire/concepts/Distance.hpp"
#include "assfire/concepts/TimeInterval.hpp"
#include "RouteInfo.hpp"
#include <vector>

namespace assfire::router {
    class TripInfo {
    public:
        using RouteInfoList = std::vector<RouteInfo>;

        TripInfo()
                :
                total_distance(Distance::zero()),
                total_duration(TimeInterval::zero()) {}

        TripInfo(const Distance &total_distance, const TimeInterval &total_duration, RouteInfoList route_info_list)
                :
                total_distance(total_distance),
                total_duration(total_duration),
                route_infos(std::move(route_info_list)) {}

        TripInfo(const TripInfo &rhs) = default;

        bool operator==(const TripInfo &ri) const = default;

        const Distance &getTotalDistance() const {
            return total_distance;
        }

        const TimeInterval &getTotalDuration() const {
            return total_duration;
        }

        const RouteInfo& operator[](std::size_t index) const {
            return route_infos[index];
        }

        bool isInfinity() const {
            return total_distance.isInfinity() || total_duration.isInfinity();
        }

        bool isZero() const {
            return total_distance.isZero() && total_duration.isZero();
        }

        static TripInfo zero() {
            return TripInfo(Distance::zero(), TimeInterval::zero(), {});
        }

        static TripInfo infinity() {
            return TripInfo(Distance::infinity(), TimeInterval::infinity(), {});
        }

    private:
        Distance total_distance;
        TimeInterval total_duration;
        RouteInfoList route_infos;
    };
}