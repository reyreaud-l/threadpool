cmake_minimum_required(VERSION 3.1)
set(LIB threadpool)
project(${LIB} CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# CMake generators if no build type is set.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELEASE)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_SUFFIX)

# Uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_MODULE_PATH}/uninstall.cmake.in"
    "${CMAKE_MODULE_PATH}/uninstall.cmake"
    IMMEDIATE @ONLY
  )

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_MODULE_PATH}/uninstall.cmake)
endif()

find_package(Threads REQUIRED)
find_package(benchmark REQUIRED)
find_package(GTest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -fsanitize=thread")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_subdirectory(include)
add_subdirectory(tests)

message(STATUS "-------------------------------")
message(STATUS "++ Setup: ")
message(STATUS "System:       " ${CMAKE_SYSTEM})
message(STATUS "Build dir:    " ${CMAKE_BINARY_DIR})
message(STATUS "Build type:   " ${CMAKE_BUILD_TYPE})
message(STATUS "CXX:          " ${CMAKE_CXX_COMPILER_ID})
message(STATUS "CXXFLAGS:     " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE_SUFFIX}})
message(STATUS "Verbose:      " ${CMAKE_VERBOSE_MAKEFILE})
message(STATUS "")
message(STATUS "++ Lib Info")
message(STATUS "ar:           " ${CMAKE_AR})
message(STATUS "ranlib:       " ${CMAKE_RANLIB})
message(STATUS "-------------------------------")
