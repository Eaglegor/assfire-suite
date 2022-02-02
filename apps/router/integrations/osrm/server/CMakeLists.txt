set(TARGET_NAME assfire-osrm-server)
set(SOURCES
        main.cpp)
#set(HEADERS
#        assfire/router/RouterService.hpp)

find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)

add_executable(assfire-osrm-server ${SOURCES})
target_link_libraries(assfire-osrm-server PRIVATE assfire-router-api assfire-router-grpc assfire-router-proto-cpp-translation cxxopts::cxxopts)

install(TARGETS assfire-router-server COMPONENT router-server)
if(ASSFIRE_RPM_ENABLED)
    cpack_add_component(router-server)
endif()
