#include <Router.hpp>

namespace assfire
{
	class EuclideanRouter: public Router {
		virtual routing::RouteInfo getRoute(routing::SingleRouteRequest) const override;
		virtual void getRoutesBatch(routing::ManyToManyRoutesRequest, std::function<void(routing::RouteInfo)>) const override;
	};
}