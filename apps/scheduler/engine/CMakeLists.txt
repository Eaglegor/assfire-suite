set(SOURCES
        assfire/scheduler/MetricsCollector.cpp
        assfire/scheduler/transport/Scheduler.cpp
        assfire/scheduler/transport/algorithms/BilinearSchedulingAlgorithm.cpp
        assfire/scheduler/transport/algorithms/BilinearSchedulingAlgorithm.hpp
        assfire/scheduler/transport/algorithms/LinearSchedulingAlgorithm.cpp
        assfire/scheduler/transport/algorithms/LinearSchedulingAlgorithm.hpp
        assfire/scheduler/transport/algorithms/BaseSchedulingAlgorithm.hpp
        assfire/scheduler/transport/algorithms/BaseSchedulingAlgorithm.cpp
        assfire/scheduler/transport/algorithms/BaseSchedulingAlgorithm.hpp)
set(HEADERS
        assfire/scheduler/transport/Scheduler.hpp
        assfire/scheduler/transport/SchedulingAlgorithm.hpp
        assfire/scheduler/MetricsCollector.hpp)

find_package(spdlog CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)

add_library(assfire-scheduler-engine ${SOURCES})
target_link_libraries(assfire-scheduler-engine PUBLIC spdlog::spdlog prometheus-cpp::core assfire-scheduler-api assfire-router-client)
target_include_directories(assfire-scheduler-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})