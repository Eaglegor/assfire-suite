#include <gtest/gtest.h>

#include "WaybillSchedulingAlgorithmFixtureBase.hpp"
#include "assfire/scheduler/engine/algorithms/LinearWaybillSchedulingAlgorithmTest.hpp"
#include "assfire/router/tests/MockRouteProviderEngine.hpp"
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

    DistanceMatrix distance_matrix = createDistanceMatrix({
                                                                  "00:10",
                                                                  "00:10",
                                                                  "00:10"
                                                          });

    Waybill::Allocations jobs = {
            a(0, d("00:15"), tw("00:00", "02:00")),
            a(1, d("00:15"), tw("00:00", "02:00")),
            a(2, d("00:15"), tw("00:00", "02:00"))
    };

    Waybill expected_waybill(tw("00:00", "23:59"), {
            a(jobs[0], t("00:00"), t("00:15"), r("00:10")),
            a(jobs[1], t("00:25"), t("00:40"), r("00:10")),
            a(jobs[2], t("00:50"), t("01:05"), r("00:00")),
    });

    Waybill scheduled_waybill(tw("00:00", "23:59"), jobs);

    LinearWaybillSchedulingAlgorithm algorithm(distance_matrix);

    algorithm.scheduleWaybill(scheduled_waybill);

    checkEquality(scheduled_waybill, expected_waybill);
}

TEST_F(LinearWaybillSchedulingAlgorithmTest, Test2)
{
    MarkdownTableParser parser;

    std::string test_table = R"(| Field1  | Field2  | Field3  | Field4  | Field5  |
                                | ------- | ------- | ------- | ------- | ------- |
                                | 11      | 12      | 13      | 14      | 15      |
                                | 21      | 22      | 23      | 24      | 25      |
                                | 31      | 32      | 33      | 34      | 35      |
                                | 41      | 42      | 43      | 44      | 45      |
                                | 51      | 52      | 53      | 54      | 55      |)";

    parser.parseTable(test_table);

    std::cout << "End!" << std::endl;
}