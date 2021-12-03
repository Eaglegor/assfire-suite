set(HEADERS
        assfire/scheduler/engine/SchedulerEngine.hpp
        assfire/scheduler/engine/algorithms/LinearWaybillSchedulingAlgorithm.hpp
        assfire/scheduler/engine/algorithms/BilinearWaybillSchedulingAlgorithm.hpp
        assfire/scheduler/engine/EngineWaybillSchedulerFactory.hpp
        assfire/scheduler/engine/MetricsCollector.hpp)

set(SOURCES
        assfire/scheduler/engine/SchedulerEngine.cpp
        assfire/scheduler/engine/algorithms/LinearWaybillSchedulingAlgorithm.cpp
        assfire/scheduler/engine/algorithms/BilinearWaybillSchedulingAlgorithm.cpp
        assfire/scheduler/engine/EngineWaybillSchedulerFactory.cpp)

find_package(spdlog CONFIG REQUIRED)
include(find_utils)
find_cpp_redis_vcpkg()

add_library(assfire-scheduler-engine ${SOURCES} ${HEADERS})
target_link_libraries(assfire-scheduler-engine PUBLIC spdlog::spdlog assfire-scheduler-api assfire-router-api)
target_include_directories(assfire-scheduler-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
