set(PUBLIC_HEADERS
        assfire/sample/client/GreeterClient.hpp)
set(SOURCES
        assfire/sample/client/GreeterClient.cpp)

add_library(assfire-sample-client ${SOURCES})

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(assfire-sample-client PRIVATE assfire-sample-api spdlog::spdlog)
target_include_directories(assfire-sample-client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS assfire-sample-client COMPONENT sample-client)
foreach(item ${PUBLIC_HEADERS})
    configure_file(${item} ${CMAKE_CURRENT_BINARY_DIR}/public_headers/${item} COPYONLY)
endforeach()
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/public_headers/assfire DESTINATION ${CMAKE_INSTALL_PREFIX}/include COMPONENT sample-client)


if(ASSFIRE_PACKAGE_BUILD_RPM)
    cpack_add_component(sample-client)
endif()