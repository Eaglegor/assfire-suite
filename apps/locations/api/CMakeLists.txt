set(SOURCES
        assfire/locations/api/Coordinate.cpp
        assfire/locations/api/Location.cpp
        assfire/locations/api/io/Streams.cpp)
set(HEADERS
        assfire/locations/api/Coordinate.hpp
        assfire/locations/api/Location.hpp
        assfire/locations/api/io/Streams.hpp)

add_library(assfire-locations-api ${SOURCES} ${HEADERS})
target_include_directories(assfire-locations-api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})