#include "MockCacheConnector.hpp"

using namespace assfire::router;

MockCacheConnector::MockCacheConnector(const std::shared_ptr<MockCache> &cache) : cache(cache) {}

void MockCacheConnector::put(const std::string &key, const std::string &payload) {
    cache->put(key, payload);
}

CacheConnector::CacheEntry MockCacheConnector::get(const std::string &key) {
    return cache->get(key);
}

void MockCacheConnector::addReply(const std::string &key, const std::string &reply) {
    cache->addReply(key, reply);
}

void MockCacheConnector::addError(const std::string &key, const std::string &error_message) {
    cache->addError(key, error_message);
}
