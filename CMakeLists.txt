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

if(NOT DEFINED ${PROJECT_PREFIX}_APPLICATION_RELEASE_VERSION)
	set(${PROJECT_PREFIX}_APPLICATION_RELEASE_VERSION "UNDEFINED")
endif()

message(STATUS "Application release version: ${${PROJECT_PREFIX}_APPLICATION_RELEASE_VERSION}")

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG True)

option(${PROJECT_PREFIX}_DEBUG_MODE "Enable debug mode" ON)
option(${PROJECT_PREFIX}_BUNDLE_INSTALL_MODE "Enable bundle mode for installation" OFF)
option(${PROJECT_PREFIX}_AUTO_INSTALL_RUNTIME_DEPS "Automatically copy all needed dynamic libraries to the installation location" OFF)
option(${PROJECT_PREFIX}_GENERATE_GRPC_REVERSE_PROXIES "Generate reverse HTTP proxy services for gRPC (go modules setup is needed)" OFF)
option(${PROJECT_PREFIX}_PACKAGE_BUILD_RPM "Build RPM packages" OFF)

option(${PROJECT_PREFIX}_APPS_BUILD_ALL "Build all applications" ON)
option(${PROJECT_PREFIX}_APPS_BUILD_${PROJECT_PREFIX}_ROUTER "Build Assfire Router" OFF)

set(${PROJECT_PREFIX}_APP_CLIENT_SUFFIX client)
set(${PROJECT_PREFIX}_APP_PROTO_BINDING_SUFFIX proto-binding)
set(${PROJECT_PREFIX}_APP_REVERSE_PROXY_SUFFIX reverse-proxy)
set(${PROJECT_PREFIX}_APP_ENGINE_SUFFIX engine)

if(${PROJECT_PREFIX}_DEBUG_MODE)
	message(STATUS "Debug mode enabled")
	add_definitions("-D${PROJECT_PREFIX}_DEBUG_MODE")
	add_definitions("-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

enable_testing()

if(${PROJECT_PREFIX}_PACKAGE_BUILD_RPM)
	message(STATUS "RPM builds enabled")
	set(CPACK_GENERATOR "RPM")
	set(CPACK_PACKAGE_VERSION ${${PROJECT_PREFIX}_APPLICATION_RELEASE_VERSION})
	set(CPACK_PACKAGE_CONTACT "Nikolay Gorbachenko <eaglegor@gmail.com>")
	set(CPACK_PACKAGE_VENDOR "Nikolay Gorbachenko <eaglegor@gmail.com>")
	set(CPACK_RPM_COMPONENT_INSTALL ON)
	set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
	include(CPack)
endif()

add_subdirectory(apps)
