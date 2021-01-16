#include "assfire/engine/router/RouteProvider.hpp"
#include "assfire/engine/router/MetricsCollector.hpp"
#include <random>

namespace assfire::router
{
	class RandomRouteProvider: public RouteProvider {
	public:
		RandomRouteProvider(MetricsCollector metrics_context = MetricsCollector());

        GetSingleRouteResponse getRoute(GetSingleRouteRequest, long) const override;
        GetRoutesBatchResponse getRoutesBatch(GetRoutesBatchRequest, long) const override;
        void getRoutesBatch(GetRoutesBatchRequest, RoutesBatchConsumer, long) const override;

	private:
		MetricsCollector metrics_collector;
		std::random_device rd;
		mutable std::mt19937 gen;
		mutable std::uniform_real_distribution<> distr;
	};
}