cmake_minimum_required(VERSION 3.11)
project(ASSFIRE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS True) # To avoid declspec(dllexport) for each symbol
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif()

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG True)

option(DEBUG_MODE "Enable debug mode" ON)
if(DEBUG_MODE)
	message(STATUS "Debug mode enabled")
	add_definitions("-DDEBUG_MODE")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

add_subdirectory(routing)
