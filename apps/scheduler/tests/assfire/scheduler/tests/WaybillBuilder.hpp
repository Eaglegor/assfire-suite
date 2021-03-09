#pragma once

#include "assfire/concepts/Location.hpp"
#include "assfire/router/api/IndexedLocation.hpp"
#include "assfire/router/api/DistanceMatrix.hpp"
#include "assfire/scheduler/api/WaybillAllocation.hpp"
#include "assfire/scheduler/api/Waybill.hpp"
#include "assfire/router/tests/MockRouteProviderEngine.hpp"
#include "assfire/router/api/distance_matrix_engines/DirectRequestDistanceMatrixEngine.hpp"
#include <memory>

namespace assfire::scheduler {
    class WaybillBuilder {
    public:
        WaybillBuilder& parse(const std::string& schedule);

        Waybill buildWaybill();
        Waybill buildCollapsedWaybill();
        router::DistanceMatrix buildDistanceMatrix();

    private:
        std::unique_ptr<router::DistanceMatrix> distance_matrix;
        TimePoint shift_start;
        TimePoint shift_end;
        Waybill::Allocations allocations;
    };
}