#pragma once

#include "assfire/scheduler/api/WaybillSchedulingAlgorithm.hpp"
#include "assfire/router/api/DistanceMatrix.hpp"

namespace assfire::scheduler {
    class LinearWaybillSchedulingAlgorithm : public WaybillSchedulingAlgorithm {
    public:
        LinearWaybillSchedulingAlgorithm(const router::DistanceMatrix &distance_matrix);

        void scheduleWaybill(Waybill &waybill) const override;

    private:
        router::DistanceMatrix distance_matrix;
    };
}


