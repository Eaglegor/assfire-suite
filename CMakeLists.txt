cmake_minimum_required(VERSION 3.11)
project(assfire)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

string(TOUPPER ${PROJECT_NAME} PROJECT_PREFIX)

if(MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS True) # To avoid declspec(dllexport) for each symbol
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /MP")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /EHsc /MP")
	add_definitions("-D_WIN32_WINNT=0x600")
	add_definitions("-DNOMINMAX")
endif()

if(NOT DEFINED ASSFIRE_APPLICATION_RELEASE_VERSION)
	set(ASSFIRE_APPLICATION_RELEASE_VERSION "UNDEFINED")
endif()

if(NOT ${ASSFIRE_APPLICATION_RELEASE_VERSION} MATCHES "v?[0-9]+\\.[0-9]+\\.[0-9]+[-.]?.*")
	message(STATUS "Using SEMVER version enforcement")
	set(ASSFIRE_APPLICATION_RELEASE_VERSION "v0.0.0-${ASSFIRE_APPLICATION_RELEASE_VERSION}")
endif()

if(NOT DEFINED ASSFIRE_HELM_INSTALL_RELEASE_NAMESPACE)
	set(ASSFIRE_HELM_INSTALL_RELEASE_NAMESPACE assfire)
endif()

message(STATUS "Application release version: ${ASSFIRE_APPLICATION_RELEASE_VERSION}")

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG True)

option(ASSFIRE_DEBUG_MODE "Enable debug mode" ON)
option(ASSFIRE_IN_SOURCE_GO_ENABLED "Build non-generated go modules in source tree (useful to edit source files directly while working with Go IDE)" OFF)

option(ASSFIRE_HELM_ENABLED "Build helm charts" ON)
option(ASSFIRE_DOCKER_ENABLED "Build docker images" ON)
option(ASSFIRE_GRPC_GATEWAYS_ENABLED "Build reverse HTTP proxy services for gRPC" ON)
option(ASSFIRE_DEMO_APPS_ENABLED "Build demo apps" ON)
option(ASSFIRE_RPM_ENABLED "Build RPM packages" ON)

option(ASSFIRE_ROUTER_MAPTEST_ENABLED "Build Assfire Router Maptest module" OFF)

if(ASSFIRE_DEBUG_MODE)
	message(STATUS "Debug mode enabled")
	add_definitions("-DASSFIRE_DEBUG_MODE")
	set(ASSFIRE_ALLOW_CORS_IN_GRPC_GATEWAY true)
	add_definitions("-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE")
else()
	set(ASSFIRE_ALLOW_CORS_IN_GRPC_GATEWAY false)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

enable_testing()

include_directories(utils)

add_custom_target(helm-package)
add_custom_target(helm-push)
add_custom_target(helm-install)
add_custom_target(helm-uninstall)
add_custom_target(helm-deploy)

add_custom_target(docker-build)
add_custom_target(docker-push)

add_subdirectory(apps)
add_subdirectory(cluster)

