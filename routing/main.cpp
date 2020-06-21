#include <routing.pb.h>
#include "RoutingService.hpp"
#include <grpc++/server_builder.h>
#include <memory>

using namespace assfire;
using namespace assfire::routing;
using namespace grpc;

int main()
{
	std::string server_address("0.0.0.0:50051");
	assfire::RoutingService service;

	ServerBuilder serverBuilder;
	serverBuilder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	serverBuilder.RegisterService(&service);
	std::unique_ptr<Server> server(serverBuilder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();
	return 0;
}