set(SOURCES

        )
set(HEADERS
        assfire/osrm/api/OsrmApi.hpp
        )

find_package(spdlog CONFIG REQUIRED)

add_library(assfire-osrm-api ${SOURCES} ${HEADERS})
target_link_libraries(assfire-osrm-api PUBLIC spdlog::spdlog assfire-router-api)
target_include_directories(assfire-osrm-api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
