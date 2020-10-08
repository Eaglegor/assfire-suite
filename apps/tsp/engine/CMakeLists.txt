set(SOURCES
        assfire/tsp/MetricsCollector.cpp
        assfire/tsp/TspSolver.cpp
        assfire/tsp/algorithms/two_opt/TwoOptTspAlgorithm.cpp)
set(HEADERS
        assfire/tsp/MetricsCollector.hpp
        assfire/tsp/TspSolver.hpp
        assfire/tsp/algorithms/model/Job.hpp
        assfire/tsp/algorithms/model/Location.hpp
        assfire/tsp/algorithms/model/Problem.hpp
        assfire/tsp/algorithms/model/Resource.hpp
        assfire/tsp/algorithms/model/RoutingOptons.hpp
        assfire/tsp/algorithms/model/Solution.hpp
        assfire/tsp/algorithms/model/Waypoint.hpp
        assfire/tsp/algorithms/two_opt/TwoOptTspAlgorithm.hpp)

find_package(spdlog CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)

add_library(assfire-tsp-engine ${SOURCES})
target_link_libraries(assfire-tsp-engine PUBLIC spdlog::spdlog prometheus-cpp::core assfire-tsp-api assfire-router-client)
target_include_directories(assfire-tsp-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})