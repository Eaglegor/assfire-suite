set(SOURCES
        main.cpp
        assfire/tsp/worker/RabbitMqTaskProcessor.cpp
        assfire/tsp/worker/StdoutSolutionPublisher.cpp
        assfire/tsp/worker/NopSavedStateManager.cpp
        assfire/tsp/worker/RedisSolutionPublisher.cpp
        )

set(HEADERS
        assfire/tsp/worker/SolutionPublisher.hpp
        assfire/tsp/worker/TaskProcessor.hpp
        assfire/tsp/worker/RabbitMqTaskProcessor.hpp
        assfire/tsp/worker/StdoutSolutionPublisher.hpp
        assfire/tsp/worker/SavedStateManager.hpp
        assfire/tsp/worker/NopSavedStateManager.hpp
        assfire/tsp/worker/RedisSolutionPublisher.hpp
        )

find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)
find_package(rabbitmq-c CONFIG REQUIRED)
include(pkg_utils)
vcpkg_find_cpp_redis()

add_executable(assfire-tsp-worker ${SOURCES} ${HEADERS})
target_link_libraries(assfire-tsp-worker PRIVATE assfire-tsp-proto assfire-tsp-proto-cpp-translation assfire-tsp-engine assfire-router-client prometheus-cpp::pull rabbitmq::rabbitmq rabbitmq::rabbitmq-static cpp_redis::cpp_redis)

install(TARGETS assfire-tsp-worker COMPONENT tsp-worker)
if (ASSFIRE_PACKAGE_BUILD_RPM)
    cpack_add_component(tsp-worker)
endif ()

#add_subdirectory(tests)