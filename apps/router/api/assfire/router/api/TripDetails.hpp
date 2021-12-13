#pragma once

#include "assfire/concepts/Distance.hpp"
#include "assfire/concepts/TimeInterval.hpp"
#include "RouteDetails.hpp"
#include <vector>

namespace assfire::router {
    class TripDetails {
    public:
        using RouteDetailsList = std::vector<RouteDetails>;

        TripDetails()
                :
                total_distance(Distance::zero()),
                total_duration(TimeInterval::zero()) {}

        TripDetails(const Distance &total_distance, const TimeInterval &total_duration, RouteDetailsList route_details_list)
                :
                total_distance(total_distance),
                total_duration(total_duration),
                route_details(std::move(route_details_list)) {}

        TripDetails(const TripDetails &rhs) = default;

        bool operator==(const TripDetails &ri) const = default;

        const Distance &getTotalDistance() const {
            return total_distance;
        }

        const TimeInterval &getTotalDuration() const {
            return total_duration;
        }

        const RouteDetails& operator[](std::size_t index) const {
            return route_details[index];
        }

        int size() const {
            return route_details.size();
        }

        bool isInfinity() const {
            return total_distance.isInfinity() || total_duration.isInfinity();
        }

        bool isZero() const {
            return total_distance.isZero() && total_duration.isZero();
        }

        static TripDetails zero() {
            return TripDetails(Distance::zero(), TimeInterval::zero(), {});
        }

        static TripDetails infinity() {
            return TripDetails(Distance::infinity(), TimeInterval::infinity(), {});
        }

    private:
        Distance total_distance;
        TimeInterval total_duration;
        RouteDetailsList route_details;
    };
}