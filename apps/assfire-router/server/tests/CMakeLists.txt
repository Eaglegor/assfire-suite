add_executable(RoutingServerTest EuclideanRouterTest.cpp main.cpp  RandomRouterTest.cpp)
add_test(unit-routing-server RoutingServerTest)

find_package(Catch2 REQUIRED)

target_link_libraries(RoutingServerTest Catch2::Catch2 assfire-router-engine)