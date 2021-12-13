#pragma once

#include <assfire/router/engine/impl/route_provider_engines/OsrmConnector.hpp>
#include <unordered_map>
#include <cpprest/json.h>

namespace assfire::router {
    class MockOsrmConnector : public OsrmConnector {
    public:
        void addReply(const utility::string_t& get_url, const web::json::value& reply);

        web::json::value requestOsrmRoute(const utility::string_t &get_url) override;

    private:
        std::unordered_map<utility::string_t, web::json::value> replies;
    };
}