#include "WaybillSchedulingAlgorithmFixtureBase.hpp"
#include "assfire/concepts/io/Streams.hpp"

namespace assfire::scheduler {

    void WaybillSchedulingAlgorithmFixtureBase::checkEquality(const Waybill &lhs, const Waybill &rhs) {
        EXPECT_EQ(lhs.getEarliestPossibleStartTime(), rhs.getEarliestPossibleStartTime());
        EXPECT_EQ(lhs.getLatestPossibleEndTime(), rhs.getLatestPossibleEndTime());

        ASSERT_EQ(lhs.size(), rhs.size());

        for (int i = 0; i < lhs.size(); ++i) {
            EXPECT_EQ(lhs[i], rhs[i]);
            EXPECT_EQ(lhs[i].getStartTime(), rhs[i].getStartTime());
            EXPECT_EQ(lhs[i].getEndTime(), rhs[i].getEndTime());
        }
    }

    Location WaybillSchedulingAlgorithmFixtureBase::getLocation(int lat, int lon) {
        return Location::fromEncodedLatLon(lat, lon);
    }

    void WaybillSchedulingAlgorithmFixtureBase::makeSimpleTest(const std::string &schedule,
                                                               const std::function<std::unique_ptr<WaybillSchedulingAlgorithm>(const router::DistanceMatrix &)> &createAlgorithm) {
        WaybillBuilder waybill_builder;
        waybill_builder.parse(schedule);

        Waybill scheduled_waybill(waybill_builder.buildCollapsedWaybill());
        Waybill expected_waybill(waybill_builder.buildWaybill());

        std::unique_ptr<WaybillSchedulingAlgorithm> algorithm = createAlgorithm(waybill_builder.buildDistanceMatrix());

        algorithm->scheduleWaybill(scheduled_waybill);

        checkEquality(scheduled_waybill, expected_waybill);
    }
}