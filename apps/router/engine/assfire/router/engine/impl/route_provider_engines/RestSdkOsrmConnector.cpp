#include "RestSdkOsrmConnector.hpp"
#include <spdlog/spdlog.h>
#include "CpprestWstringConvert.hpp"

using namespace assfire::router;
using namespace web;
using namespace web::http;
using namespace web::http::client;

RestSdkOsrmConnector::RestSdkOsrmConnector(const std::string &endpoint) :
        client(std::make_unique<web::http::client::http_client>(convert(endpoint))) {

}

web::json::value RestSdkOsrmConnector::requestOsrmRoute(const utility::string_t &get_url) {
    return client->request(methods::GET, get_url).then([](const http_response &response) {
        SPDLOG_DEBUG("OSRM response: {}", convert(response.to_string()));
        return response.extract_json();
    }).get();
}
