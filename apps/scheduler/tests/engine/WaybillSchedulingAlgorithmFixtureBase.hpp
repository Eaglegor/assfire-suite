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
    class WaybillSchedulingAlgorithmFixtureBase : public testing::Test {
    public:
        static Location getLocation(int lat, int lon);

        static void checkEquality(const Waybill& lhs, const Waybill& rhs);

        static void makeSimpleTest(const std::string& schedule, const std::function<std::unique_ptr<WaybillSchedulingAlgorithm>(const router::DistanceMatrix& dm)>& createAlgorithm);
    };
}