set(SOURCES
        main.cpp
        assfire/tsp/TspService.cpp)
set(HEADERS
        assfire/tsp/TspService.hpp)

find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)

add_executable(assfire-tsp-server ${SOURCES})
target_link_libraries(assfire-tsp-server PRIVATE assfire-router-client assfire-scheduler-engine assfire-tsp-grpc assfire-tsp-proto-cpp-translation prometheus-cpp::pull)

install(TARGETS assfire-scheduler-server COMPONENT scheduler-server)
if(ASSFIRE_PACKAGE_BUILD_RPM)
    cpack_add_component(scheduler-server)
endif()

#add_subdirectory(tests)