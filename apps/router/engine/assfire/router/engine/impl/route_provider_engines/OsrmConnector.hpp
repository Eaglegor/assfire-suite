#pragma once

#include <cpprest/json.h>

namespace assfire::router
{
    class OsrmConnector {
    public:
        virtual ~OsrmConnector() = default;
        virtual web::json::value requestRouteInfo(const utility::string_t& get_url) = 0;
    };
}