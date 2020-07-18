#include "Router.hpp"
#include "RoutingMetricsCollector.hpp"

namespace assfire
{
	class EuclideanRouter: public Router {
	public:
		EuclideanRouter(RoutingMetricsCollector metrics_context = RoutingMetricsCollector());
		virtual routing::proto::RouteInfo getRoute(routing::proto::SingleRouteRequest, long) const override;
		virtual void getRoutesBatch(routing::proto::ManyToManyRoutesRequest, std::function<void(routing::proto::RouteInfo)>, long) const override;

	private:
		RoutingMetricsCollector metrics_context;
	};
}