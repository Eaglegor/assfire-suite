set(PUBLIC_HEADERS
        assfire/router/client/RouterClient.hpp
        assfire/router/client/transport/GrpcProtobufClient.hpp
        assfire/router/client/transport/ProtobufClient.hpp
        assfire/router/client/GrpcRouteProviderEngine.hpp
        assfire/router/client/ClientDistanceMatrixFactory.hpp
        )
set(SOURCES
        assfire/router/client/RouterClient.cpp
        assfire/router/client/transport/GrpcProtobufClient.cpp
        assfire/router/client/GrpcRouteProviderEngine.cpp
        assfire/router/client/ClientDistanceMatrixFactory.cpp)


add_library(assfire-router-client ${SOURCES} ${HEADERS})

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(assfire-router-client PUBLIC assfire-router-grpc assfire-router-api assfire-router-proto-cpp-translation PRIVATE spdlog::spdlog)
target_include_directories(assfire-router-client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS assfire-router-client COMPONENT router-client)
foreach (item ${PUBLIC_HEADERS})
    configure_file(${item} ${CMAKE_CURRENT_BINARY_DIR}/public_headers/${item} COPYONLY)
endforeach ()
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/public_headers/assfire DESTINATION ${CMAKE_INSTALL_PREFIX}/include COMPONENT router-client)

if (ASSFIRE_RPM_ENABLED)
    cpack_add_component(router-client)
endif ()