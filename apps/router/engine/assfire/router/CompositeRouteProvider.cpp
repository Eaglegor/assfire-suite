#include "CompositeRouteProvider.hpp"
#include <spdlog/spdlog.h>

using namespace assfire::router;

RouteProvider::GetSingleRouteResponse CompositeRouteProvider::getRoute(GetSingleRouteRequest request, long i) const
{
    return resolveProvider(request.options(), i).getRoute(request, i);
}

RouteProvider::GetRoutesBatchResponse CompositeRouteProvider::getRoutesBatch(GetRoutesBatchRequest request, long i) const
{
    return resolveProvider(request.options(), i).getRoutesBatch(request, i);
}

void CompositeRouteProvider::getRoutesBatch(GetRoutesBatchRequest request, RoutesBatchConsumer consumer, long i) const
{
    resolveProvider(request.options(), i).getRoutesBatch(request, consumer, i);
}

const RouteProvider &CompositeRouteProvider::resolveProvider(const RouteProvider::RoutingOptions &options, long request_id) const
{
    auto iter = providers.find(options.routing_type());
    if (iter == providers.end()) {
        SPDLOG_ERROR("Can't resolve route provider for the request {}", request_id);
        throw std::invalid_argument("Can't resolve route provider for the request");
    }
    return *iter->second;
}
