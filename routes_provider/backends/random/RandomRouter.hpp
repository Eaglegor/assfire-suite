#include "Router.hpp"
#include "RoutingMetricsCollector.hpp"
#include <random>

namespace assfire
{
	class RandomRouter: public Router {
	public:
		RandomRouter(RoutingMetricsCollector metrics_context = RoutingMetricsCollector());
		virtual routing::proto::RouteInfo getRoute(routing::proto::SingleRouteRequest, long) const override;
		virtual void getRoutesBatch(routing::proto::ManyToManyRoutesRequest, std::function<void(routing::proto::RouteInfo)>, long) const override;

	private:
		RoutingMetricsCollector metrics_context;
		std::random_device rd;
		mutable std::mt19937 gen;
		mutable std::uniform_real_distribution<> distr;
	};
}