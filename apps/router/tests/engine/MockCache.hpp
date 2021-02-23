#include <assfire/router/engine/impl/route_provider_engines/CacheConnector.hpp>
#include <unordered_map>

namespace assfire::router {
    class MockCache {
    public:
        void put(const std::string &key, const std::string &payload);
        CacheConnector::CacheEntry get(const std::string &key);

        void addReply(const std::string& key, const std::string& reply);
        void addError(const std::string& key, const std::string& error_message);

    private:
        std::unordered_map<std::string, std::string> replies;
        std::unordered_map<std::string, std::string> errors;
    };
}