set(SOURCES

        )
set(HEADERS
        assfire/router/api/osrm/OsrmSettings.hpp
        )

find_package(spdlog CONFIG REQUIRED)

add_library(assfire-osrm-api ${SOURCES} ${HEADERS})
target_link_libraries(assfire-osrm-api PUBLIC spdlog::spdlog assfire-router-api)
target_include_directories(assfire-osrm-api PUBLIC .)
