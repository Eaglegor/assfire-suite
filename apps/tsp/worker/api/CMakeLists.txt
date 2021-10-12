set(SOURCES
        assfire/tsp/worker/task/RabbitMQWorkerTaskPublisher.cpp
        assfire/tsp/worker/solution/RedisWorkerSolutionStorage.cpp)
set(HEADERS
        assfire/tsp/worker/task/WorkerTaskPublisher.hpp
        assfire/tsp/worker/task/RabbitMQWorkerTaskPublisher.hpp
        assfire/tsp/worker/solution/WorkerSolutionStorage.hpp
        assfire/tsp/worker/solution/RedisWorkerSolutionStorage.hpp)

find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)

add_library(assfire-tsp-worker-api ${SOURCES})
target_link_libraries(assfire-tsp-worker-api PRIVATE assfire-tsp-proto prometheus-cpp::pull)
target_include_directories(assfire-tsp-worker-api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#add_subdirectory(tests)