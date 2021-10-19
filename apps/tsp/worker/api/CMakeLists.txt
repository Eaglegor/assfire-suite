set(SOURCES
        assfire/tsp/worker/RabbitMQSolutionPublisher.cpp
)
set(HEADERS

        assfire/tsp/worker/SolutionPublisher.hpp
        assfire/tsp/worker/RabbitMQSolutionPublisher.hpp
)

find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)
find_package(amqpcpp CONFIG REQUIRED)

add_library(assfire-tsp-worker-api ${SOURCES})
target_link_libraries(assfire-tsp-worker-api PRIVATE assfire-tsp-proto prometheus-cpp::pull amqpcpp)
target_include_directories(assfire-tsp-worker-api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#add_subdirectory(tests)