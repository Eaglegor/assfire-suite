add_library(assfire-concepts-api
        assfire/concepts/TimeInterval.hpp assfire/concepts/TimeInterval.cpp
        assfire/concepts/TimePoint.hpp assfire/concepts/TimePoint.cpp
        assfire/concepts/Speed.hpp assfire/concepts/Speed.cpp
        assfire/concepts/Distance.hpp assfire/concepts/Distance.cpp
        assfire/concepts/Coordinate.hpp assfire/concepts/Coordinate.cpp assfire/concepts/io/Streams.cpp)

target_include_directories(assfire-concepts-api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
