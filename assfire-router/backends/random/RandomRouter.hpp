#include "Router.hpp"
#include "RoutingMetricsCollector.hpp"
#include <random>

namespace assfire
{
	class RandomRouter: public Router {
	public:
		RandomRouter(RoutingMetricsCollector metrics_context = RoutingMetricsCollector());

        routing::proto::v1::GetSingleRouteResponse getRoute(routing::proto::v1::GetSingleRouteRequest, long) const override;
        routing::proto::v1::GetRoutesBatchResponse getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest, long) const override;
        void getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest, RoutesBatchConsumer, long) const override;

	private:
		RoutingMetricsCollector metrics_context;
		std::random_device rd;
		mutable std::mt19937 gen;
		mutable std::uniform_real_distribution<> distr;
	};
}