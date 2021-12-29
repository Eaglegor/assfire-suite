set(SOURCES
        main.cpp
        assfire/tsp/worker/impl/StdoutSolutionPublisher.cpp
        assfire/tsp/worker/impl/NopSavedStateManager.cpp
        assfire/tsp/worker/impl/RedisSolutionPublisher.cpp
        assfire/tsp/worker/impl/RabbitMqConnector.cpp
        assfire/tsp/worker/Worker.cpp
        assfire/tsp/worker/impl/RedisTaskProvider.cpp
        assfire/tsp/worker/impl/AmqpInterruptListener.cpp
        assfire/tsp/worker/impl/AmqpStatusPublisher.cpp
        assfire/tsp/worker/impl/AmqpTaskQueueListener.cpp
        assfire/tsp/worker/impl/RedisSavedStateManager.cpp assfire/tsp/worker/impl/TspWorkerKeys.cpp)

set(HEADERS
        TspAmqpTopology.hpp
        assfire/tsp/worker/SolutionPublisher.hpp
        assfire/tsp/worker/TaskProcessor.hpp
        assfire/tsp/worker/impl/StdoutSolutionPublisher.hpp
        assfire/tsp/worker/SavedStateManager.hpp
        assfire/tsp/worker/impl/NopSavedStateManager.hpp
        assfire/tsp/worker/impl/RedisSolutionPublisher.hpp
        assfire/tsp/worker/impl/TspWorkerConstants.hpp
        assfire/tsp/worker/impl/RabbitMqConnector.hpp
        assfire/tsp/worker/TaskProvider.hpp
        assfire/tsp/worker/InterruptListener.hpp
        assfire/tsp/worker/StatusPublisher.hpp
        assfire/tsp/worker/Worker.hpp
        assfire/tsp/worker/TaskQueueListener.hpp
        assfire/tsp/worker/impl/RedisTaskProvider.hpp
        assfire/tsp/worker/impl/AmqpInterruptListener.hpp
        assfire/tsp/worker/impl/AmqpStatusPublisher.hpp
        assfire/tsp/worker/impl/AmqpTaskQueueListener.hpp
        assfire/tsp/worker/impl/RedisSavedStateManager.hpp assfire/tsp/worker/impl/TspWorkerKeys.hpp assfire/tsp/worker/impl/RedisConnectionCallback.hpp assfire/tsp/worker/impl/RedisConnectionCallback.cpp)

find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)
find_package(crossguid CONFIG REQUIRED)
include(find_utils)
find_cpp_redis_vcpkg()

add_executable(assfire-tsp-worker ${SOURCES} ${HEADERS})


target_link_libraries(assfire-tsp-worker PRIVATE assfire-tsp-proto assfire-tsp-proto-cpp-translation assfire-tsp-engine assfire-router-client prometheus-cpp::pull cpp_redis::cpp_redis cxxopts::cxxopts crossguid assfire-amqp-connector assfire-redis-connector)

install(TARGETS assfire-tsp-worker COMPONENT tsp-worker)
if (ASSFIRE_RPM_ENABLED)
    cpack_add_component(tsp-worker)
endif ()

if(ASSFIRE_DOCKER_ENABLED)
    include(docker_utils)
    define_docker_target(
            TARGET_NAME assfire-tsp-worker-docker
            DOCKERFILE Dockerfile
            IMAGE_NAME assfire-tsp-worker
            IMAGE_TAG ${ASSFIRE_APPLICATION_RELEASE_VERSION}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()


#add_subdirectory(tests)

