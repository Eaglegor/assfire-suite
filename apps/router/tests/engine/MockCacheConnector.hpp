#include <assfire/router/engine/impl/route_provider_engines/CacheConnector.hpp>
#include "MockCache.hpp"
#include <unordered_map>

namespace assfire::router {
    class MockCacheConnector : public CacheConnector {
    public:
        MockCacheConnector(const std::shared_ptr<MockCache> &cache);

        void put(const std::string &key, const std::string &payload) override;

        CacheEntry get(const std::string &key) override;

        void addReply(const std::string& key, const std::string& reply);
        void addError(const std::string& key, const std::string& error_message);

    private:
        std::shared_ptr<MockCache> cache;
    };
}