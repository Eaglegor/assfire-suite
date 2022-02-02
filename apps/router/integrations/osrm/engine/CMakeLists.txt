set(SOURCES
        assfire/router/engine/osrm/LibOsrmEngine.cpp
        assfire/router/engine/osrm/OsrmHttpEngine.cpp
        )
set(HEADERS
        assfire/router/engine/osrm/LibOsrmEngine.hpp
        assfire/router/engine/osrm/OsrmHttpEngine.hpp
        )

find_package(spdlog CONFIG REQUIRED)

add_library(assfire-osrm-engine ${SOURCES} ${HEADERS})
target_link_libraries(assfire-osrm-engine PUBLIC spdlog::spdlog assfire-osrm-api)
target_include_directories(assfire-osrm-engine PUBLIC .)
