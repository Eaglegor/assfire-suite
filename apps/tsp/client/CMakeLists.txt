set(PUBLIC_HEADERS
        assfire/tsp/client/TspClient.hpp)
set(SOURCES
        assfire/tsp/client/TspClient.cpp)

add_library(assfire-tsp-client ${SOURCES})

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(assfire-tsp-client PRIVATE assfire-tsp-api spdlog::spdlog)
target_include_directories(assfire-tsp-client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS assfire-tsp-client COMPONENT tsp-client)
foreach(item ${PUBLIC_HEADERS})
    configure_file(${item} ${CMAKE_CURRENT_BINARY_DIR}/public_headers/${item} COPYONLY)
endforeach()
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/public_headers/assfire DESTINATION ${CMAKE_INSTALL_PREFIX}/include COMPONENT tsp-client)


if(ASSFIRE_PACKAGE_BUILD_RPM)
    cpack_add_component(tsp-client)
endif()