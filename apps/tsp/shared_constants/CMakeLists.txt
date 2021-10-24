add_library(assfire-tsp-shared-constants INTERFACE
        assfire/tsp/TspAmqpConstants.hpp)
target_include_directories(assfire-tsp-shared-constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})