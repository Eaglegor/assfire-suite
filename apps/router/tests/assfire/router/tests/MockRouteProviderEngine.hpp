#pragma once

#include <queue>
#include <unordered_map>
#include <string>

#include <assfire/router/api/RouteProviderEngine.hpp>
#include <assfire/router/api/IndexedLocation.hpp>

namespace assfire::router {
    class MockRouteProviderEngine : public RouteProviderEngine {
    public:
        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;
        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

        void addResponse(double distance, long duration);
        void addResponse(double distance, long duration, const RouteDetails::Waypoints& waypoints);

        void addResponse(const Location& from, const Location& to, const RouteDetails& details);

        int getCallsCount() const;

    private:
        static std::string buildKey(const Location& from, const Location& to) ;

        mutable std::queue<RouteDetails> route_details_responses;
        std::unordered_map<std::string, RouteDetails> indexed_responses;
        mutable int calls_count = 0;
    };
}