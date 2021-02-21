#include "MockOsrmConnector.hpp"
#include <assfire/engine/router/impl/route_provider_engines/CpprestWstringConvert.hpp>

using namespace assfire::router;

void MockOsrmConnector::addReply(const utility::string_t &get_url, const web::json::value &reply) {
    replies.emplace(get_url, reply);
}

web::json::value MockOsrmConnector::requestRouteInfo(const utility::string_t &get_url) {
    if(replies.contains(get_url)) {
        return replies.at(get_url);
    } else {
        web::json::value error_response;
        error_response[convert("code")] = web::json::value("InvalidUrl");
        error_response[convert("message")] = web::json::value("Mocked URL is not found");
        return error_response;
    }
}
