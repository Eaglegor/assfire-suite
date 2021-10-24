set(SOURCES
        main.cpp
        assfire/tsp/TspService.cpp
        assfire/tsp/RedisSolutionStorage.cpp
        assfire/tsp/RabbitMQWorkerTransport.cpp
        assfire/tsp/NopTspTaskStorage.cpp
        assfire/tsp/RedisTspTaskStorage.cpp)
set(HEADERS
        assfire/tsp/TspService.hpp
        assfire/tsp/TaskIdGenerator.hpp
        assfire/tsp/IncrementalTaskIdGenerator.hpp
        assfire/tsp/SolutionStorage.hpp
        assfire/tsp/RedisSolutionStorage.hpp
        assfire/tsp/WorkerTransport.hpp
        assfire/tsp/RabbitMQWorkerTransport.hpp
        assfire/tsp/TspTaskStorage.hpp
        assfire/tsp/NopTspTaskStorage.hpp
        assfire/tsp/RedisTspTaskStorage.cpp)

find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)
include(pkg_utils)
vcpkg_find_cpp_redis()

add_executable(assfire-tsp-server ${SOURCES} ${HEADERS})
target_link_libraries(assfire-tsp-server PRIVATE spdlog::spdlog assfire-tsp-grpc prometheus-cpp::pull assfire-util-amqp-connector assfire-tsp-shared-constants cpp_redis::cpp_redis)

install(TARGETS assfire-tsp-server COMPONENT tsp-server)
if (ASSFIRE_PACKAGE_BUILD_RPM)
    cpack_add_component(tsp-server)
endif ()

#add_subdirectory(tests)