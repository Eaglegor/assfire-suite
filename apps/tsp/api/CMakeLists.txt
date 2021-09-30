set(SOURCES
        assfire/tsp/api/NopTspInterruptor.cpp
        )
set(HEADERS
        assfire/tsp/api/NopTspInterruptor.hpp
        assfire/tsp/api/TspSolution.hpp
        assfire/tsp/api/TspTask.hpp
        assfire/tsp/api/TspPoint.hpp
        assfire/tsp/api/TspValidationResult.hpp
        assfire/tsp/api/TspCost.hpp
        assfire/tsp/api/TspCostFunction.hpp
        assfire/tsp/api/TspAlgorithmType.hpp
        assfire/tsp/api/TspSolverSettings.hpp
        assfire/tsp/api/TspCostFunctionType.hpp
        assfire/tsp/api/TspCostFunctionSettings.hpp
        assfire/tsp/api/TspValidatorType.hpp
        assfire/tsp/api/TspValidatorSettings.hpp
        assfire/tsp/api/TspEstimatorSettings.hpp
        assfire/tsp/api/TspSolverApi.hpp
        assfire/tsp/api/TspResultCallback.hpp
        assfire/tsp/api/TspRoutingSettings.hpp
        )

find_package(spdlog CONFIG REQUIRED)

add_library(assfire-tsp-api ${SOURCES} ${HEADERS})
target_link_libraries(assfire-tsp-api PUBLIC spdlog::spdlog assfire-router-api assfire-scheduler-api)
target_include_directories(assfire-tsp-api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})