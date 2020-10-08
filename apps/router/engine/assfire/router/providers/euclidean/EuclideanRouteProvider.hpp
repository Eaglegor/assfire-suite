#include "assfire/router/RouteProvider.hpp"
#include "assfire/router/MetricsCollector.hpp"

namespace assfire::router
{
	class EuclideanRouteProvider: public RouteProvider {
	public:
		explicit EuclideanRouteProvider(MetricsCollector metrics_context = MetricsCollector());

        GetSingleRouteResponse getRoute(GetSingleRouteRequest, long) const override;
        GetRoutesBatchResponse getRoutesBatch(GetRoutesBatchRequest, long) const override;
        void getRoutesBatch(GetRoutesBatchRequest, RoutesBatchConsumer, long) const override;

	private:
		MetricsCollector metrics_collector;
	};
}