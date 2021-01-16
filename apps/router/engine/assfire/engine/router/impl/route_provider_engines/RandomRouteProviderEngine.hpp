#pragma once

#include <assfire/engine/router/model/RouteInfo.hpp>
#include <assfire/engine/router/model/Location.hpp>
#include <assfire/engine/router/model/RouteDetails.hpp>
#include "../../RouteProviderEngine.hpp"
#include "../../RoutingProfile.hpp"
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