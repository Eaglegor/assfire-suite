set(PUBLIC_HEADERS
        assfire/scheduler/transport/client/SchedulerClient.hpp)
set(SOURCES
        assfire/scheduler/transport/client/SchedulerClient.cpp)

add_library(assfire-scheduler-client ${SOURCES})

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(assfire-scheduler-client PRIVATE assfire-scheduler-api spdlog::spdlog)
target_include_directories(assfire-scheduler-client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS assfire-scheduler-client COMPONENT scheduler-client)
foreach(item ${PUBLIC_HEADERS})
    configure_file(${item} ${CMAKE_CURRENT_BINARY_DIR}/public_headers/${item} COPYONLY)
endforeach()
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/public_headers/assfire DESTINATION ${CMAKE_INSTALL_PREFIX}/include COMPONENT scheduler-client)


if(ASSFIRE_PACKAGE_BUILD_RPM)
    cpack_add_component(scheduler-client)
endif()