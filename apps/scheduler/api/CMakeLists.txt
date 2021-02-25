set(SOURCES
        assfire/scheduler/api/main.cpp
        )
set(HEADERS
        assfire/scheduler/api/model/WaybillSchedulable.hpp
        assfire/scheduler/api/model/WaybillRepresentable.hpp)

find_package(spdlog CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
include(pkg_utils)
vcpkg_find_cpp_redis()

add_library(assfire-scheduler-api ${SOURCES} ${HEADERS})
target_link_libraries(assfire-scheduler-api PUBLIC spdlog::spdlog assfire-router-api)
target_include_directories(assfire-scheduler-api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})