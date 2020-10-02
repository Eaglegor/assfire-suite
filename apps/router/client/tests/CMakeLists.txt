find_package(Catch2 REQUIRED)

add_executable(RoutesProviderClientServerIntegrationTest ClientIntegrationTest.cpp main.cpp)
target_link_libraries(RoutesProviderClientServerIntegrationTest Catch2::Catch2 ${ASSFIRE_ROUTER_APP_NAME}-client)

add_executable(RoutesProviderRedisIntegrationTest main.cpp "RedisCacheIntegrationTest.cpp")
target_link_libraries(RoutesProviderRedisIntegrationTest Catch2::Catch2 ${ASSFIRE_ROUTER_APP_NAME}-client)

add_test(integration-routes-provider-client-server RoutesProviderClientServerIntegrationTest)
add_test(integration-routes-provider-redis-cache RoutesProviderRedisIntegrationTest)



