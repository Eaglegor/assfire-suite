set(SOURCES
        main.cpp
        assfire/scheduler/SchedulerService.cpp)
set(HEADERS
        assfire/scheduler/SchedulerService.hpp)

find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)

add_executable(assfire-scheduler-server ${SOURCES})
target_link_libraries(assfire-scheduler-server PRIVATE assfire-router-client assfire-scheduler-engine assfire-scheduler-api assfire-scheduler-grpc assfire-scheduler-proto-cpp-translation prometheus-cpp::pull cxxopts::cxxopts)

install(TARGETS assfire-scheduler-server COMPONENT scheduler-server)
if(ASSFIRE_RPM_ENABLED)
    cpack_add_component(scheduler-server)
endif()

#add_subdirectory(tests)