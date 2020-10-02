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

if(ASSFIRE_APPLICATION_RELEASE_VERSION MATCHES "v*\\d+\\.\\d+\\.\\d+[-.].*")
	set(ASSFIRE_APPLICATION_RELEASE_VERSION_SEMVER_ENFORCED ${ASSFIRE_APPLICATION_RELEASE_VERSION})
else()
	message(STATUS "Using SEMVER version enforcement")
	set(ASSFIRE_APPLICATION_RELEASE_VERSION_SEMVER_ENFORCED "0.0.0-${ASSFIRE_APPLICATION_RELEASE_VERSION}")
	message(STATUS "Enforced version:  ${ASSFIRE_APPLICATION_RELEASE_VERSION_SEMVER_ENFORCED}")
endif()


message(STATUS "Application release version: ${ASSFIRE_APPLICATION_RELEASE_VERSION}")

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG True)

option(ASSFIRE_DEBUG_MODE "Enable debug mode" ON)
option(ASSFIRE_BUNDLE_INSTALL_MODE "Enable bundle mode for installation" OFF)
option(ASSFIRE_AUTO_INSTALL_RUNTIME_DEPS "Automatically copy all needed dynamic libraries to the installation location" OFF)
option(ASSFIRE_GENERATE_GRPC_REVERSE_PROXIES "Generate reverse HTTP proxy services for gRPC (go modules setup is needed)" OFF)
option(ASSFIRE_PACKAGE_BUILD_RPM "Build RPM packages" OFF)

option(ASSFIRE_APPS_BUILD_ALL "Build all applications" ON)
option(ASSFIRE_APPS_BUILD_ASSFIRE_ROUTER "Build Assfire Router" OFF)

set(ASSFIRE_APP_CLIENT_SUFFIX client)
set(ASSFIRE_APP_API_SUFFIX api)
set(ASSFIRE_APP_GATEWAY_SUFFIX gateway)
set(ASSFIRE_APP_ENGINE_SUFFIX engine)
set(ASSFIRE_APP_TOOLS_SUFFIX tools)

if(ASSFIRE_DEBUG_MODE)
	message(STATUS "Debug mode enabled")
	add_definitions("-DASSFIRE_DEBUG_MODE")
	add_definitions("-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE")
	set(ASSFIRE_ALLOW_CORS_IN_GATEWAY true)
else()
	set(ASSFIRE_ALLOW_CORS_IN_GATEWAY false)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

enable_testing()

if(ASSFIRE_PACKAGE_BUILD_RPM)
	message(STATUS "RPM builds enabled")
	set(CPACK_GENERATOR "RPM")
	set(CPACK_PACKAGE_VERSION ${ASSFIRE_APPLICATION_RELEASE_VERSION})
	set(CPACK_PACKAGE_CONTACT "Nikolay Gorbachenko <eaglegor@gmail.com>")
	set(CPACK_PACKAGE_VENDOR "Nikolay Gorbachenko <eaglegor@gmail.com>")
	set(CPACK_RPM_COMPONENT_INSTALL ON)
	set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
	include(CPack)
endif()

add_subdirectory(api-model)
add_subdirectory(apps)
