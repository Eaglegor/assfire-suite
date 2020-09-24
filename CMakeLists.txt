cmake_minimum_required(VERSION 3.11)
project(ASSFIRE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS True) # To avoid declspec(dllexport) for each symbol
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
	add_definitions("-D_WIN32_WINNT=0x600")
endif()

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG True)

option(ASSFIRE_DEBUG_MODE "Enable debug mode" ON)
option(ASSFIRE_BUNDLE_INSTALL_MODE "Enable bundle mode for installation" OFF)
option(ASSFIRE_AUTO_INSTALL_RUNTIME_DEPS "Automatically copy all needed dynamic libraries to the installation location" OFF)
option(ASSFIRE_GENERATE_GRPC_REVERSE_PROXIES "Generate reverse HTTP proxy services for gRPC (go modules setup is needed)" OFF)
if(ASSFIRE_DEBUG_MODE)
	message(STATUS "Debug mode enabled")
	add_definitions("-DASSFIRE_DEBUG_MODE")
	add_definitions("-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

enable_testing()

add_subdirectory(routes_provider)
