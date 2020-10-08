set(TARGET_NAME assfire-tsp-server)
set(SOURCES
        main.cpp
        assfire/tsp/TspService.cpp)
set(HEADERS
        assfire/tsp/TspService.hpp)

find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)

add_executable(assfire-tsp-server ${SOURCES})
target_link_libraries(assfire-tsp-server PRIVATE assfire-tsp-engine assfire-tsp-api prometheus-cpp::pull)

install(TARGETS assfire-tsp-server COMPONENT tsp-server)
if(ASSFIRE_PACKAGE_BUILD_RPM)
    cpack_add_component(tsp-server)
endif()

#add_subdirectory(tests)