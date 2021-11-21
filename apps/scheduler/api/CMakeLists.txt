set(SOURCES
        assfire/scheduler/api/TimeWindow.cpp
        assfire/scheduler/api/Waybill.cpp
        assfire/scheduler/api/WaybillAllocation.cpp
        assfire/scheduler/api/WaybillScheduler.cpp
        )
set(HEADERS
        assfire/scheduler/api/WaybillSchedulerSettings.hpp
        assfire/scheduler/api/TimeWindow.hpp
        assfire/scheduler/api/WaybillScheduler.hpp
        assfire/scheduler/api/Waybill.hpp
        assfire/scheduler/api/WaybillSchedulingAlgorithm.hpp
        assfire/scheduler/api/WaybillAllocation.hpp
        assfire/scheduler/api/WaybillSchedulingAlgorithmType.hpp)

find_package(spdlog CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
include(pkg_utils)
vcpkg_find_cpp_redis()

add_library(assfire-scheduler-api ${SOURCES} ${HEADERS})
target_link_libraries(assfire-scheduler-api PUBLIC spdlog::spdlog assfire-router-api)
target_include_directories(assfire-scheduler-api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})