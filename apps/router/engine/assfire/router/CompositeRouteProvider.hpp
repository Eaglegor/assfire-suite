#pragma once

#include "RouteProvider.hpp"
#include "MetricsCollector.hpp"
#include <type_traits>

namespace assfire::router
{
    class CompositeRouteProvider : public RouteProvider
    {
    public:
        GetSingleRouteResponse getRoute(GetSingleRouteRequest, long i) const override;

        GetRoutesBatchResponse getRoutesBatch(GetRoutesBatchRequest, long i) const override;

        void getRoutesBatch(GetRoutesBatchRequest, RoutesBatchConsumer consumer, long i) const override;

        void addProvider(RoutingType type, std::unique_ptr<RouteProvider> &&provider) {
            providers.emplace(type, std::move(provider));
        }

    private:
        const RouteProvider& resolveProvider(const RoutingOptions&, long) const;
        std::unordered_map<RoutingType, std::unique_ptr<RouteProvider>> providers;
    };
}