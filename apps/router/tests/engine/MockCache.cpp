#include "MockCache.hpp"
#include <spdlog/spdlog.h>

using namespace assfire::router;

void MockCache::put(const std::string &key, const std::string &payload) {
    addReply(key, payload);
}

CacheConnector::CacheEntry MockCache::get(const std::string &key) {
    CacheConnector::CacheEntry result;
    if(errors.contains(key)) {
        result.is_error = true;
        result.message = errors.at(key);
    } else if(replies.contains(key)) {
        result.is_present = true;
        result.payload = replies.at(key);
    }
    return result;
}

void MockCache::addReply(const std::string &key, const std::string &reply) {
    replies.insert_or_assign(key, reply);
    SPDLOG_INFO("Registered cache entry: ({})->{}", key, reply);
}

void MockCache::addError(const std::string &key, const std::string &error_message) {
    errors.insert_or_assign(key, error_message);
    SPDLOG_INFO("Registered error entry: ({})->{}", key, error_message);
}
