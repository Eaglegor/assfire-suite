#include "Router.hpp"
#include "RoutingMetricsCollector.hpp"

namespace assfire
{
	class CrowflightRouter: public Router {
	public:
        CrowflightRouter(RoutingMetricsCollector metrics_context = RoutingMetricsCollector());

        routing::proto::v1::GetSingleRouteResponse getRoute(routing::proto::v1::GetSingleRouteRequest, long) const override;
        routing::proto::v1::GetRoutesBatchResponse getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest, long) const override;
        void getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest, RoutesBatchConsumer, long) const override;

	private:
		RoutingMetricsCollector metrics_context;
	};
}