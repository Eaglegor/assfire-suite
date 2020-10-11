set(SOURCES
        assfire/estimator/MetricsCollector.cpp
        assfire/estimator/transport/ScheduleEstimator.cpp
        assfire/estimator/transport/cost_functions/TotalDistanceCostFunction.cpp
        assfire/estimator/transport/cost_functions/TotalDistanceCostFunction.hpp
        assfire/estimator/transport/cost_functions/TotalTimeCostFunction.cpp
        assfire/estimator/transport/cost_functions/TotalTimeCostFunction.hpp
        assfire/estimator/transport/cost_functions/ResourceUsageCostFunction.cpp
        assfire/estimator/transport/cost_functions/ResourceUsageCostFunction.hpp)
set(HEADERS
        assfire/estimator/transport/ScheduleEstimator.hpp
        assfire/estimator/transport/CostFunction.hpp
        assfire/estimator/MetricsCollector.hpp)

find_package(spdlog CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)

add_library(assfire-estimator-engine ${SOURCES})
target_link_libraries(assfire-estimator-engine PUBLIC spdlog::spdlog prometheus-cpp::core assfire-estimator-api assfire-router-client)
target_include_directories(assfire-estimator-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})