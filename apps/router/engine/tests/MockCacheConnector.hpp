#include <assfire/engine/router/impl/route_provider_engines/CacheConnector.hpp>
#include <unordered_map>

namespace assfire::router {
    class MockCacheConnector : public CacheConnector {
    public:
        void put(const std::string &key, const std::string &payload) override;

        CacheEntry get(const std::string &key) override;

        void addReply(const std::string& key, const std::string& reply);
        void addError(const std::string& key, const std::string& error_message);

    private:
        std::unordered_map<std::string, std::string> replies;
        std::unordered_map<std::string, std::string> errors;
    };
}