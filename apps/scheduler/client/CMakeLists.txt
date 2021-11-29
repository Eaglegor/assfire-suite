set(PUBLIC_HEADERS
        assfire/scheduler/SchedulerClient.hpp
        assfire/scheduler/ClientWaybillSchedulerFactory.hpp
        assfire/scheduler/SchedulerGrpcConnector.hpp
        assfire/scheduler/RemoteSchedulerGrpcConnector.hpp
        assfire/scheduler/RemoteWaybillSchedulingAlgorithm.hpp
        )
set(SOURCES
        assfire/scheduler/SchedulerClient.cpp
        assfire/scheduler/RemoteSchedulerGrpcConnector.cpp
        assfire/scheduler/ClientWaybillSchedulerFactory.cpp
        assfire/scheduler/RemoteWaybillSchedulingAlgorithm.cpp
        )

add_library(assfire-scheduler-client ${SOURCES})

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(assfire-scheduler-client PRIVATE assfire-scheduler-api assfire-scheduler-grpc assfire-scheduler-proto-cpp-translation spdlog::spdlog)
target_include_directories(assfire-scheduler-client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS assfire-scheduler-client COMPONENT scheduler-client)
foreach(item ${PUBLIC_HEADERS})
    configure_file(${item} ${CMAKE_CURRENT_BINARY_DIR}/public_headers/${item} COPYONLY)
endforeach()
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/public_headers/assfire DESTINATION ${CMAKE_INSTALL_PREFIX}/include COMPONENT scheduler-client)


if(ASSFIRE_RPM_ENABLED)
    cpack_add_component(scheduler-client)
endif()