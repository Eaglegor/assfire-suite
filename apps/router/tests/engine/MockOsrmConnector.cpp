#include "MockOsrmConnector.hpp"
#include <assfire/router/engine/impl/route_provider_engines/CpprestWstringConvert.hpp>
#include <spdlog/spdlog.h>

using namespace assfire::router;

void MockOsrmConnector::addReply(const utility::string_t &get_url, const web::json::value &reply) {
    replies.emplace(get_url, reply);
    SPDLOG_INFO("Registered reply: ({})->{}", convert(get_url), convert(reply.serialize()));
}

web::json::value MockOsrmConnector::requestOsrmRoute(const utility::string_t &get_url) {
    if(replies.contains(get_url)) {
        return replies.at(get_url);
    } else {
        web::json::value error_response;
        error_response[convert("code")] = web::json::value::string(convert("InvalidUrl"));
        error_response[convert("message")] = web::json::value::string(convert("Mocked URL is not found: ") + get_url);
        return error_response;
    }
}
