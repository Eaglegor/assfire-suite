#pragma once

#include "assfire/scheduler/api/WaybillSchedulingAlgorithm.hpp"
#include "assfire/router/api/DistanceMatrix.hpp"

namespace assfire::scheduler {
    class BilinearWaybillSchedulingAlgorithm : public WaybillSchedulingAlgorithm {
    public:
        BilinearWaybillSchedulingAlgorithm(const router::DistanceMatrix &distance_matrix);

        void scheduleWaybill(Waybill &waybill) const override;

    private:
        router::DistanceMatrix distance_matrix;
    };
}