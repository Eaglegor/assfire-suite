#pragma once
#include "assfire/router/api/DistanceMatrix.hpp"
#include "assfire/router/api/RoutingProfile.hpp"
#include "assfire/router/api/RouteProviderSettings.hpp"
#include "assfire/router/api/RouterEngineType.hpp"
#include <functional>

namespace assfire::scheduler {
    class WaybillSchedulingContext {
    public:
        using DistanceMatrixProvider = std::function<router::DistanceMatrix(router::RouterEngineType, const router::RouteProviderSettings&, const router::RoutingProfile&)>;

        WaybillSchedulingContext(const DistanceMatrixProvider &distance_matrix_provider) :
        distance_matrix_provider(distance_matrix_provider) {}

        const DistanceMatrixProvider& getDistanceMatrixProvider() const
        {
            return distance_matrix_provider;
        }

    private:
        DistanceMatrixProvider distance_matrix_provider;
    };
}
