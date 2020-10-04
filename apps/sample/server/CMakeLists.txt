set(SOURCES
        main.cpp
        assfire/sample/GreeterService.cpp)
set(HEADERS
        assfire/sample/GreeterService.hpp)

find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)

add_executable(assfire-sample-server ${SOURCES})
target_link_libraries(assfire-sample-server PRIVATE assfire-sample-engine assfire-sample-api prometheus-cpp::pull)

install(TARGETS assfire-sample-server COMPONENT sample-server)
if(ASSFIRE_PACKAGE_BUILD_RPM)
    cpack_add_component(sample-server)
endif()

#add_subdirectory(tests)