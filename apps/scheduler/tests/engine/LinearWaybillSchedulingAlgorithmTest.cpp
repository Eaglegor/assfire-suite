#include <gtest/gtest.h>

#include "WaybillSchedulingAlgorithmFixtureBase.hpp"
#include "assfire/scheduler/engine/algorithms/LinearWaybillSchedulingAlgorithmTest.hpp"
#include "assfire/router/tests/MockRouteProviderEngine.hpp"
#include "assfire/scheduler/tests/WaybillBuilder.hpp"
#include "assfire/scheduler/tests/io/markdown/MarkdownTableParser.hpp"

using namespace assfire;
using namespace assfire::router;
using namespace assfire::scheduler;
using namespace std;

class LinearWaybillSchedulingAlgorithmTest : public WaybillSchedulingAlgorithmFixtureBase
{
public:

};

TEST_F(LinearWaybillSchedulingAlgorithmTest, Test1)
{

    DistanceMatrix distance_matrix = createDistanceMatrix({"00:10", "00:10", "00:10"});

    Waybill::Allocations jobs = {
            allocation(0, d("00:15"), tw("00:00", "02:00")),
            allocation(1, d("00:15"), tw("00:00", "02:00")),
            allocation(2, d("00:15"), tw("00:00", "02:00"))
    };

    Waybill expected_waybill(tw("00:00", "23:59"), {
            allocation(jobs[0], t("00:00"), t("00:15"), route("00:10")),
            allocation(jobs[1], t("00:25"), t("00:40"), route("00:10")),
            allocation(jobs[2], t("00:50"), t("01:05"), route("00:00")),
    });

    Waybill scheduled_waybill(tw("00:00", "23:59"), jobs);

    LinearWaybillSchedulingAlgorithm algorithm(distance_matrix);

    algorithm.scheduleWaybill(scheduled_waybill);

    checkEquality(scheduled_waybill, expected_waybill);
}

TEST_F(LinearWaybillSchedulingAlgorithmTest, DISABLED_Test2) {

    std::string schedule = R"(| Type                   | Allocated time  | Time window     |
                              | --------------------   | --------------- | --------------- |
                              | ### SHIFT_START ###### | 00:00           |                 |
                              | ALLOCATION             | 00:00 - 00:15   | [00:00 - 02:00] |
                              | ROUTE                  | 00:15 - 00:25   |                 |
                              | ALLOCATION             | 00:25 - 00:40   | [00:00 - 02:00] |
                              | ROUTE                  | 00:40 - 00:50   |                 |
                              | ALLOCATION             | 00:50 - 01:05   | [00:00 - 02:00] |
                              | ROUTE                  | 01:05           |                 |
                              | ### SHIFT_END ######## | 23:59           |                 |)";

    WaybillBuilder waybill_builder;
    waybill_builder.parse(schedule);

    Waybill scheduled_waybill(waybill_builder.buildCollapsedWaybill());
    Waybill expected_waybill(waybill_builder.buildWaybill());

    LinearWaybillSchedulingAlgorithm algorithm(waybill_builder.buildDistanceMatrix());

    algorithm.scheduleWaybill(scheduled_waybill);

    checkEquality(scheduled_waybill, expected_waybill);
}