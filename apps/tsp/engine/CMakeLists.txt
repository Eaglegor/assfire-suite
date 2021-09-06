set(HEADERS
        assfire/tsp/engine/TspSolverFactory.hpp
        assfire/tsp/engine/algorithms/TwoOptTspAlgorithm.hpp
        )

set(SOURCES
        assfire/tsp/engine/TspSolverFactory.cpp
        assfire/tsp/engine/algorithms/TwoOptTspAlgorithm.cpp)

find_package(spdlog CONFIG REQUIRED)

add_library(assfire-tsp-engine ${SOURCES} ${HEADERS})
target_link_libraries(assfire-tsp-engine PUBLIC spdlog::spdlog assfire-tsp-api assfire-scheduler-api assfire-router-api)
target_include_directories(assfire-tsp-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
