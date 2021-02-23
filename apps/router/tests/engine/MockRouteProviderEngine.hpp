#pragma once

#include <queue>
#include <assfire/api/router/RouteProviderEngine.hpp>

namespace assfire::router {
    class MockRouteProviderEngine : public RouteProviderEngine {
    public:
        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;

        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

        void addResponse(double distance, long duration);
        void addResponse(double distance, long duration, const RouteDetails::Waypoints& waypoints);

        int getCallsCount() const;

    private:
        mutable std::queue<RouteDetails> route_details_responses;
        mutable int calls_count = 0;
    };
}