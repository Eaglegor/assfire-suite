set(SOURCES
        assfire/locations/api/Coordinate.cpp
        assfire/locations/api/Location.cpp)
set(HEADERS
        assfire/locations/api/Coordinate.hpp
        assfire/locations/api/Location.hpp)

add_library(assfire-locations-api ${SOURCES} ${HEADERS})
target_include_directories(assfire-locations-api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})