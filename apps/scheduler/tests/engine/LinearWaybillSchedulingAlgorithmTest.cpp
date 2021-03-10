#include <gtest/gtest.h>

#include "WaybillSchedulingAlgorithmFixtureBase.hpp"
#include "assfire/scheduler/engine/algorithms/LinearWaybillSchedulingAlgorithmTest.hpp"


using namespace assfire;
using namespace assfire::router;
using namespace assfire::scheduler;
using namespace std;

class LinearWaybillSchedulingAlgorithmTest : public WaybillSchedulingAlgorithmFixtureBase {
public:

};

TEST_F(LinearWaybillSchedulingAlgorithmTest, AllocationsWithoutTimeWindowsAreScheduledOneAfterAnother) {
    makeSimpleTest(R"(| Type                   | Allocated time  |
                              | --------------------   | --------------- |
                              | ### SHIFT_START ###### | 00:00           |
                              | ALLOCATION             | 00:00 - 00:15   |
                              | ROUTE                  | 00:15 - 00:25   |
                              | ALLOCATION             | 00:25 - 00:30   |
                              | ALLOCATION             | 00:30 - 00:40   |
                              | ROUTE                  | 00:40 - 00:50   |
                              | ALLOCATION             | 00:50 - 01:05   |
                              | ### SHIFT_END ######## | 23:59           |)",
                   [](const DistanceMatrix &dm) {
                       return std::make_unique<LinearWaybillSchedulingAlgorithm>(dm);
                   }
    );
}

TEST_F(LinearWaybillSchedulingAlgorithmTest, AllocationsAreScheduledWithWaitingTimeIfTimeWindowNotYetStarted) {
    makeSimpleTest(R"(| Type                   | Allocated time  | Time window     |
                              | --------------------   | --------------- | --------------- |
                              | ### SHIFT_START ###### | 00:00           |                 |
                              | ALLOCATION             | 00:00 - 00:15   | [00:00 - 02:00] |
                              | ROUTE                  | 00:15 - 00:25   |                 |
                              | ALLOCATION             | 00:25 - 00:30   | [00:00 - 02:00] |
                              | WAITING >>>>>>>>>>>>>> | 00:30 - 00:35   |                 |
                              | ALLOCATION             | 00:35 - 00:45   | [00:35 - 02:00] |
                              | ROUTE                  | 00:45 - 00:55   |                 |
                              | WAITING >>>>>>>>>>>>>> | 00:55 - 01:00   |                 |
                              | ALLOCATION             | 01:00 - 01:10   | [01:00 - 02:00] |
                              | ### SHIFT_END ######## | 23:59           |                 |)",
                   [](const DistanceMatrix &dm) {
                       return std::make_unique<LinearWaybillSchedulingAlgorithm>(dm);
                   }
    );
}

TEST_F(LinearWaybillSchedulingAlgorithmTest, AllocationsAreScheduledWithLateIfTimeWindowAreTooTight) {
    makeSimpleTest(R"(| Type                   | Allocated time  | Time window     |
                              | --------------------   | --------------- | --------------- |
                              | ### SHIFT_START ###### | 00:00           |                 |
                              | ALLOCATION             | 00:00 - 00:15   | [00:00 - 02:00] |
                              | ROUTE                  | 00:15 - 00:25   |                 |
                              | ALLOCATION             | 00:25 - 00:30   | [00:00 - 02:00] |
                              | WAITING >>>>>>>>>>>>>> | 00:30 - 00:40   |                 |
                              | ALLOCATION             | 00:40 - 00:45   | [00:40 - 02:00] |
                              | ROUTE                  | 00:45 - 01:20   |                 |
                              | ALLOCATION             | 01:20 - 01:30   | [01:00 - 01:10] | [//]: # Late by 10 mins
                              | ### SHIFT_END ######## | 23:59           |                 |)",
                   [](const DistanceMatrix &dm) {
                       return std::make_unique<LinearWaybillSchedulingAlgorithm>(dm);
                   }
    );
}