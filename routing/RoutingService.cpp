#include "RoutingService.hpp"

using namespace assfire;

grpc::Status assfire::RoutingService::GetSingleRoute(grpc::ServerContext* context, const routing::SingleRouteRequest* request, routing::RouteInfo* response)
{
	try {
		const Router& router = selectRouter(request->options());
		response->CopyFrom(router.getRoute(*request));
	}
	catch (std::invalid_argument e)
	{
		return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
	}
	
	return grpc::Status::OK;
}

grpc::Status assfire::RoutingService::GetRoutesBatch(grpc::ServerContext* context, const routing::ManyToManyRoutesRequest* request, grpc::ServerWriter<routing::RouteInfo>* stream)
{
	try {
		const Router& router = selectRouter(request->options());
		router.getRoutesBatch(*request, [stream](routing::RouteInfo result) {stream->Write(result); });
	}
	catch (std::invalid_argument e)
	{
		return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
	}

	return grpc::Status::OK;
}

grpc::Status assfire::RoutingService::GetStreamingRoutesBatch(grpc::ServerContext* context, grpc::ServerReaderWriter<routing::RouteInfo, routing::ManyToManyRoutesRequest>* stream)
{
	try {
		routing::ManyToManyRoutesRequest request;
		while (stream->Read(&request))
		{
			const Router& router = selectRouter(request.options());
			router.getRoutesBatch(request, [stream](routing::RouteInfo result) {stream->Write(result); });
		}
	}
	catch (std::invalid_argument e)
	{
		return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
	}

	return grpc::Status::OK;
}

const Router& assfire::RoutingService::selectRouter(const routing::RoutingOptions& options) const
{
	if (options.has_euclidean_routing_options()) return std::get<EuclideanRouter>(routers);
	throw std::invalid_argument("Requested router is unsupported");
}
