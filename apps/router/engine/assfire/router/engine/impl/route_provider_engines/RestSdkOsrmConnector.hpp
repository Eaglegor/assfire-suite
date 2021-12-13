#pragma once

#include "OsrmConnector.hpp"
#include <cpprest/http_client.h>

namespace assfire::router
{
    class RestSdkOsrmConnector : public OsrmConnector {
    public:
        RestSdkOsrmConnector(const std::string& endpoint);

        web::json::value requestOsrmRoute(const utility::string_t &get_url) override;

    private:
        std::unique_ptr<web::http::client::http_client> client;
    };
}