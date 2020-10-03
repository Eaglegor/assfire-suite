#include "assfire/router/RouteProvider.hpp"
#include "assfire/router/MetricsCollector.hpp"
#include <assfire/api/v1/model/routing/routing.pb.h>

namespace assfire::router
{
	class CrowflightRouteProvider: public RouteProvider {
	public:
        explicit CrowflightRouteProvider(MetricsCollector metrics_context = MetricsCollector());

        GetSingleRouteResponse getRoute(GetSingleRouteRequest, long) const override;
        GetRoutesBatchResponse getRoutesBatch(GetRoutesBatchRequest, long) const override;
        void getRoutesBatch(GetRoutesBatchRequest, RoutesBatchConsumer, long) const override;

	private:
		MetricsCollector metrics_collector;
	};
}