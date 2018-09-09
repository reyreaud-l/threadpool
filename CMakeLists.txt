cmake_minimum_required(VERSION 3.9)
set(LIB threadpool)
project(${LIB} LANGUAGES CXX VERSION 1.0)

option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# If no build type is set, set to release.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELEASE)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_SUFFIX)

# Threads kinda of is required (obviously)
find_package(Threads REQUIRED)

# Compilation settings (for tests mainly)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_subdirectory(include)

# Tests
if ((BUILD_TESTS) OR (NOT CMAKE_BUILD_TYPE MATCHES "RELEASE"))
  add_subdirectory(tests)
endif()

# Examples
if (BUILD_EXAMPLES)
  add_subdirectory(doc/src/examples)
endif()

message(STATUS "-------------------------------")
message(STATUS "++ Setup: ")
message(STATUS "System:        " ${CMAKE_SYSTEM})
message(STATUS "Build dir:     " ${CMAKE_BINARY_DIR})
message(STATUS "Build type:    " ${CMAKE_BUILD_TYPE})
message(STATUS "CXX:           " ${CMAKE_CXX_COMPILER_ID})
message(STATUS "CXXFLAGS:      " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE_SUFFIX}})
message(STATUS "Verbose:       " ${CMAKE_VERBOSE_MAKEFILE})
message(STATUS "")
message(STATUS "++ Lib Info")
message(STATUS "ar:            " ${CMAKE_AR})
message(STATUS "ranlib:        " ${CMAKE_RANLIB})
message(STATUS "++ Options")
message(STATUS "Tests:         " ${BUILD_TESTS})
message(STATUS "-------------------------------")
