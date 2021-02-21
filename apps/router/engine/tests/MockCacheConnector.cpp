#include "MockCacheConnector.hpp"

using namespace assfire::router;

void MockCacheConnector::put(const std::string &key, const std::string &payload) {
    addReply(key, payload);
}

CacheConnector::CacheEntry MockCacheConnector::get(const std::string &key) {
    CacheEntry result;
    if(errors.contains(key)) {
        result.is_error = true;
        result.message = errors.at(key);
    } else if(replies.contains(key)) {
        result.is_present = true;
        result.payload = replies.at(key);
    }
    return result;
}

void MockCacheConnector::addReply(const std::string &key, const std::string &reply) {
    replies.emplace(key, reply);
}

void MockCacheConnector::addError(const std::string &key, const std::string &error_message) {
    errors.emplace(key, error_message);
}
