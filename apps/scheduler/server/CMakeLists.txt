set(SOURCES
        main.cpp
        assfire/scheduler/transport/SchedulerService.cpp)
set(HEADERS
        assfire/scheduler/transport/SchedulerService.hpp)

find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)

add_executable(assfire-scheduler-server ${SOURCES})
target_link_libraries(assfire-scheduler-server PRIVATE assfire-router-client assfire-scheduler-engine assfire-scheduler-api assfire-scheduler-proto-cpp-translation prometheus-cpp::pull)

install(TARGETS assfire-scheduler-server COMPONENT scheduler-server)
if(ASSFIRE_PACKAGE_BUILD_RPM)
    cpack_add_component(scheduler-server)
endif()

#add_subdirectory(tests)