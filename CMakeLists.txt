cmake_minimum_required(VERSION 3.11)
project(assfire)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

string(TOUPPER ${PROJECT_NAME} PROJECT_PREFIX)

if(MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS True) # To avoid declspec(dllexport) for each symbol
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
	add_definitions("-D_WIN32_WINNT=0x600")
endif()

if(NOT DEFINED ASSFIRE_APPLICATION_RELEASE_VERSION)
	set(ASSFIRE_APPLICATION_RELEASE_VERSION "UNDEFINED")
endif()

if(NOT ASSFIRE_APPLICATION_RELEASE_VERSION MATCHES "v*\\d+\\.\\d+\\.\\d+[-.].*")
	message(STATUS "Using SEMVER version enforcement")
	set(ASSFIRE_APPLICATION_RELEASE_VERSION "0.0.0-${ASSFIRE_APPLICATION_RELEASE_VERSION}")
endif()

message(STATUS "Application release version: ${ASSFIRE_APPLICATION_RELEASE_VERSION}")

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG True)

option(ASSFIRE_DEBUG_MODE "Enable debug mode" ON)
option(ASSFIRE_BUNDLE_INSTALL_MODE "Enable bundle mode for installation" OFF)
option(ASSFIRE_AUTO_INSTALL_RUNTIME_DEPS "Automatically copy all needed dynamic libraries to the installation location" OFF)
option(ASSFIRE_GENERATE_GRPC_GATEWAYS "Generate reverse HTTP proxy services for gRPC (needs golang compiler to be installed)" OFF)
option(ASSFIRE_PACKAGE_BUILD_RPM "Build RPM packages" OFF)

if(ASSFIRE_GENERATE_GRPC_GATEWAYS)
	find_program(GO_EXECUTABLE go)
	if(GO_EXECUTABLE STREQUAL "GO_EXECUTABLE-NOTFOUND")
		message(STATUS "Golang executable not found. Generating gRPC gateways will be disabled")
		set(ASSFIRE_GENERATE_GRPC_GATEWAYS OFF)
	else()
		message(STATUS "Golang executable found: ${GO_EXECUTABLE}")
	endif()
endif()

option(ASSFIRE_APPS_BUILD_ALL "Build all applications" ON)
option(ASSFIRE_APPS_BUILD_SAMPLE "Build Assfire Sample Application" OFF)

if(ASSFIRE_DEBUG_MODE)
	message(STATUS "Debug mode enabled")
	add_definitions("-DASSFIRE_DEBUG_MODE")
	add_definitions("-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE")
	set(ASSFIRE_ALLOW_CORS_IN_GRPC_GATEWAY true)
else()
	set(ASSFIRE_ALLOW_CORS_IN_GRPC_GATEWAY false)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

enable_testing()

include_directories(utils)

add_subdirectory(api-model)
add_subdirectory(apps)
