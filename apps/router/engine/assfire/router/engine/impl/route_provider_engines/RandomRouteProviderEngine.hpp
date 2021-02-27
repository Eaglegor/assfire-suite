#pragma once

#include <assfire/router/api/model/RouteInfo.hpp>
#include "assfire/concepts/Location.hpp"
#include <assfire/router/api/model/RouteDetails.hpp>
#include "assfire/router/api/RouteProviderEngine.hpp"
#include "assfire/router/api/model/RoutingProfile.hpp"
#include <random>

namespace assfire::router {
    class RandomRouteProviderEngine : public RouteProviderEngine {
    public:
        RandomRouteProviderEngine(const RoutingProfile &routingProfile);

        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;
        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

    private:
        RoutingProfile routing_profile;
        std::random_device rd;
        mutable std::mt19937 gen;
        mutable std::uniform_real_distribution<> distr;
    };
}