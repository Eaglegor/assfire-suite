set(SOURCES
        main.cpp
        assfire/tsp/worker/impl/RabbitMqTaskProcessor.cpp
        assfire/tsp/worker/impl/StdoutSolutionPublisher.cpp
        assfire/tsp/worker/impl/NopSavedStateManager.cpp
        assfire/tsp/worker/impl/RedisSolutionPublisher.cpp
        assfire/tsp/worker/impl/RabbitMqControlChannelListener.cpp
        assfire/tsp/worker/impl/RabbitMqConnector.cpp
        assfire/tsp/worker/Worker.cpp
        assfire/tsp/worker/impl/RedisTaskProvider.cpp
        assfire/tsp/worker/impl/AmqpInterruptListener.cpp)

set(HEADERS
        assfire/tsp/worker/SolutionPublisher.hpp
        assfire/tsp/worker/TaskProcessor.hpp
        assfire/tsp/worker/impl/RabbitMqTaskProcessor.hpp
        assfire/tsp/worker/impl/StdoutSolutionPublisher.hpp
        assfire/tsp/worker/SavedStateManager.hpp
        assfire/tsp/worker/impl/NopSavedStateManager.hpp
        assfire/tsp/worker/impl/RedisSolutionPublisher.hpp
        assfire/tsp/worker/impl/RabbitMqControlChannelListener.hpp
        assfire/tsp/worker/impl/TspImplConstants.hpp
        assfire/tsp/worker/impl/RabbitMqConnector.hpp
        assfire/tsp/worker/TaskProvider.hpp
        assfire/tsp/worker/InterruptListener.hpp
        assfire/tsp/worker/StatusPublisher.hpp
        assfire/tsp/worker/Worker.hpp
        assfire/tsp/worker/TaskQueueListener.hpp
        assfire/tsp/worker/impl/RedisTaskProvider.hpp
        assfire/tsp/worker/impl/AmqpInterruptListener.hpp)

find_package(rabbitmq-c CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)
include(pkg_utils)
vcpkg_find_cpp_redis()

add_executable(assfire-tsp-worker ${SOURCES} ${HEADERS})

if (WIN32)
    set(RABBIT_MQ_TARGETS rabbitmq::rabbitmq rabbitmq::rabbitmq-static)
else ()
    set(RABBIT_MQ_TARGETS rabbitmq::rabbitmq-static)
endif ()

target_link_libraries(assfire-tsp-worker PRIVATE assfire-tsp-proto assfire-tsp-proto-cpp-translation assfire-tsp-engine assfire-router-client prometheus-cpp::pull cpp_redis::cpp_redis ${RABBIT_MQ_TARGETS})

install(TARGETS assfire-tsp-worker COMPONENT tsp-worker)
if (ASSFIRE_PACKAGE_BUILD_RPM)
    cpack_add_component(tsp-worker)
endif ()

#add_subdirectory(tests)