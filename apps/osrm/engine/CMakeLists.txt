set(SOURCES
        assfire/osrm/engine/OsrmEngine.cpp
        )
set(HEADERS
        assfire/osrm/engine/OsrmEngine.hpp
        )

find_package(spdlog CONFIG REQUIRED)

add_library(assfire-osrm-engine ${SOURCES} ${HEADERS})
target_link_libraries(assfire-osrm-engine PUBLIC spdlog::spdlog assfire-osrm-api)
target_include_directories(assfire-osrm-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
