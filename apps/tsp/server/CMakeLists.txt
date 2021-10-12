set(SOURCES
        main.cpp
        assfire/tsp/TspService.cpp)
set(HEADERS
        assfire/tsp/TspService.hpp
        assfire/tsp/TaskIdGenerator.hpp
        assfire/tsp/IncrementalTaskIdGenerator.hpp)

find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)

add_executable(assfire-tsp-server ${SOURCES} ${HEADERS})
target_link_libraries(assfire-tsp-server PRIVATE spdlog::spdlog assfire-tsp-grpc prometheus-cpp::pull assfire-tsp-worker-api)

install(TARGETS assfire-tsp-server COMPONENT tsp-server)
if(ASSFIRE_PACKAGE_BUILD_RPM)
    cpack_add_component(tsp-server)
endif()

#add_subdirectory(tests)