#pragma once

#include <gtest/gtest.h>
#include <iomanip>
#include <chrono>
#include <tuple>
#include <ostream>
#include <ctime>
#include "assfire/concepts/Location.hpp"
#include "assfire/router/api/IndexedLocation.hpp"
#include "assfire/router/api/DistanceMatrix.hpp"
#include "assfire/scheduler/api/WaybillAllocation.hpp"
#include "assfire/scheduler/api/Waybill.hpp"
#include "assfire/router/tests/MockRouteProviderEngine.hpp"
#include "assfire/router/api/distance_matrix_engines/DirectRequestDistanceMatrixEngine.hpp"

namespace assfire::scheduler {
    std::ostream& operator<<(std::ostream& os, const TimePoint& tp) {
        std::time_t time = tp.toEpochSeconds();
        return os << std::put_time(std::gmtime(&time), "%H:%M:%S");
    }

    class WaybillSchedulingAlgorithmFixtureBase : public testing::Test {
    public:
        static Location getLocation(int lat, int lon) {
            return Location::fromEncodedLatLon(lat, lon);
        }

        static TimePoint t(const char *tp) {
            std::tm t = {};
            t.tm_year = 90;
            t.tm_mon = 0;
            t.tm_mday = 1;
            std::istringstream ss(tp);
            ss >> std::get_time(&t, "%H:%M");
            return TimePoint::fromEpochSeconds(std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::from_time_t(std::mktime(&t))).time_since_epoch().count());
        }

        static TimeInterval d(const char *tp) {
            std::tm t = {};
            t.tm_year = 70;
            t.tm_mon = 0;
            t.tm_mday = 1 + 10;
            std::istringstream ss(tp);
            ss >> std::get_time(&t, "%H:%M");
            std::time_t tt = std::mktime(&t) - 3600 * 240 - _timezone;
            std::chrono::time_point ctp = std::chrono::system_clock::from_time_t(tt);
            auto cctp = std::chrono::time_point_cast<std::chrono::seconds>(ctp);
            int seconds = cctp.time_since_epoch().count();
            return TimeInterval::fromSeconds(seconds);
        }

        static router::RouteInfo route(const char *tp) {
            TimeInterval duration = d(tp);
            Distance distance = Distance::fromMeters(duration.toSeconds());

            return router::RouteInfo(distance, duration);
        }

        static WaybillAllocation allocation(const WaybillAllocation &prototype, const TimePoint &start_time, const TimePoint &end_time, router::RouteInfo next_route_duration) {
            WaybillAllocation result(start_time, end_time, prototype.getPlannedDuration(), prototype.getTimeWindows(), prototype.getLocation());
            result.setNextRouteInfo(next_route_duration);
            return result;
        }

        static WaybillAllocation allocation(int location, const TimeInterval &duration, const TimeWindow &time_window) {
            return WaybillAllocation(TimePoint::zero(), TimePoint::zero(), duration, {time_window},
                                     router::IndexedLocation(location, 0, getLocation(location, location)));
        }

        static WaybillAllocation allocation(int location, const TimePoint &start, const TimePoint &end, const TimeInterval &duration, const TimeWindow &time_window) {
            return WaybillAllocation(start, end, duration, {time_window},
                                     router::IndexedLocation(location, 0, getLocation(location, location)));
        }

        static TimeWindow tw(const char *start, const char *end) {
            return TimeWindow(t(start), t(end));
        }

        void populateWaybill(Waybill &w, const Waybill::Allocations &allocations) {
            for (const auto &alloc : allocations) {
                w.push_back(alloc);
            }
        }

        router::DistanceMatrix createDistanceMatrix(const std::vector<std::string> &routes) {

            std::unique_ptr<router::MockRouteProviderEngine> route_engine = std::make_unique<router::MockRouteProviderEngine>();
            for(int i = 0; i < routes.size(); ++i) {
                TimeInterval duration = d(routes[i].c_str());
                Distance distance = Distance::fromMeters(duration.toSeconds());
                Location origin = Location::fromEncodedLatLon(i, i);
                Location destination = Location::fromEncodedLatLon(i+1, i+1);
                route_engine->addResponse(origin, destination, router::RouteDetails(router::RouteInfo(distance, duration), {origin, destination}));
            }

            return router::DistanceMatrix(std::make_unique<router::DirectRequestDistanceMatrixEngine>(std::move(route_engine), router::DistanceMatrixEngine::Tag(0)));
        }

        void checkEquality(const Waybill& lhs, const Waybill& rhs) {
            EXPECT_EQ(lhs.getEarliestPossibleStartTime(), rhs.getEarliestPossibleStartTime());
            EXPECT_EQ(lhs.getLatestPossibleEndTime(), rhs.getLatestPossibleEndTime());

            ASSERT_EQ(lhs.size(), rhs.size());

            for(int i = 0; i < lhs.size(); ++i) {
                EXPECT_EQ(lhs[i], rhs[i]);
                EXPECT_EQ(lhs[i].getStartTime(), rhs[i].getStartTime());
                EXPECT_EQ(lhs[i].getEndTime(), rhs[i].getEndTime());
            }
        }
    };
}