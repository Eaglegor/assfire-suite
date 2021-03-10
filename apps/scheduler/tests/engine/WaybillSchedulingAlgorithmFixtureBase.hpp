#pragma once

#include <gtest/gtest.h>
#include <iomanip>
#include <chrono>
#include <tuple>
#include <ostream>
#include <ctime>
#include <assfire/scheduler/api/WaybillSchedulingAlgorithm.hpp>
#include "assfire/concepts/Location.hpp"
#include "assfire/router/api/IndexedLocation.hpp"
#include "assfire/router/api/DistanceMatrix.hpp"
#include "assfire/scheduler/api/WaybillAllocation.hpp"
#include "assfire/scheduler/api/Waybill.hpp"
#include "assfire/router/tests/MockRouteProviderEngine.hpp"
#include "assfire/router/api/distance_matrix_engines/DirectRequestDistanceMatrixEngine.hpp"
#include "assfire/router/tests/MockRouteProviderEngine.hpp"
#include "assfire/scheduler/tests/WaybillBuilder.hpp"

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

        void makeSimpleTest(const std::string& schedule, const std::function<std::unique_ptr<WaybillSchedulingAlgorithm>(const router::DistanceMatrix& dm)>& createAlgorithm) {
            WaybillBuilder waybill_builder;
            waybill_builder.parse(schedule);

            Waybill scheduled_waybill(waybill_builder.buildCollapsedWaybill());
            Waybill expected_waybill(waybill_builder.buildWaybill());

            std::unique_ptr<WaybillSchedulingAlgorithm> algorithm = createAlgorithm(waybill_builder.buildDistanceMatrix());

            algorithm->scheduleWaybill(scheduled_waybill);

            checkEquality(scheduled_waybill, expected_waybill);
        }
    };
}