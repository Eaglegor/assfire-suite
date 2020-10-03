set(TARGET_NAME assfire-sample-server)
set(SOURCES
        main.cpp
        GreeterService.cpp )
set(HEADERS
        GreeterService.h)

find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)

add_executable(assfire-sample-server ${SOURCES})
target_link_libraries(${TARGET_NAME} PRIVATE assfire-sample-engine assfire-sample-api prometheus-cpp::pull)

install(TARGETS assfire-sample-server COMPONENT sample-server)
if(ASSFIRE_PACKAGE_BUILD_RPM)
    cpack_add_component(sample-server)
endif()

#add_subdirectory(tests)