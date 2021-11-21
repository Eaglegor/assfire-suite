set(HEADERS
        assfire/tsp/engine/TspEstimator.hpp
        assfire/tsp/engine/TspSolverEngine.hpp
        assfire/tsp/engine/TspAlgorithm.hpp
        assfire/tsp/engine/TspAlgorithmStateContainer.hpp
        assfire/tsp/engine/algorithms/TwoOptTspAlgorithm.hpp
        assfire/tsp/engine/EngineTspSolutionController.hpp
        assfire/tsp/engine/TspCostFunction.hpp
        assfire/tsp/engine/TspValidator.hpp
        assfire/tsp/engine/NopTspValidator.hpp
        assfire/tsp/engine/TotalDistanceTspCostFunction.hpp
        assfire/tsp/engine/algorithms/TimeWastingTspAlgorithm.hpp)

set(SOURCES
        assfire/tsp/engine/TspEstimator.cpp
        assfire/tsp/engine/TspSolverEngine.cpp
        assfire/tsp/engine/algorithms/TwoOptTspAlgorithm.cpp
        assfire/tsp/engine/EngineTspSolutionController.cpp
        assfire/tsp/engine/TspAlgorithmStateContainer.cpp
        assfire/tsp/engine/NopTspValidator.cpp
        assfire/tsp/engine/TotalDistanceTspCostFunction.cpp
        assfire/tsp/engine/algorithms/TimeWastingTspAlgorithm.cpp)

find_package(spdlog CONFIG REQUIRED)
find_package(crossguid CONFIG REQUIRED)

add_library(assfire-tsp-engine ${SOURCES} ${HEADERS})
target_link_libraries(assfire-tsp-engine PUBLIC spdlog::spdlog assfire-tsp-api assfire-scheduler-api assfire-router-api assfire-tsp-proto PRIVATE crossguid)
target_include_directories(assfire-tsp-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
