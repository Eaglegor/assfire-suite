set(TARGET_NAME assfire-router-server)
set(SOURCES
        main.cpp
        assfire/router/RouterService.cpp
        assfire/router/ServerMetricsCollector.cpp)
set(HEADERS
        assfire/router/RouterService.h
        assfire/router/ServerMetricsCollector.hpp)

find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)

add_executable(assfire-router-server ${SOURCES})
target_link_libraries(assfire-router-server PRIVATE assfire-router-engine assfire-router-api assfire-router-proto assfire-router-proto-cpp-translation prometheus-cpp::pull)

install(TARGETS assfire-router-server COMPONENT router-server)
if(ASSFIRE_PACKAGE_BUILD_RPM)
    cpack_add_component(router-server)
endif()

#add_subdirectory(tests)