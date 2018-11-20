cmake_minimum_required(VERSION 3.9)
set(LIB threadpool)
project(${LIB} LANGUAGES CXX VERSION 2.0.0)

option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)

option(DOWNLOAD_DEPENDENCIES "Download dependencies" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# If no build type is set, set to release.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELEASE)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_SUFFIX)

# Threads kinda of is required (obviously)
find_package(Threads REQUIRED)
find_program(ClangTidy NAMES "clang-tidy" DOC "Path to clang-tidy executable")

# Compilation settings (for tests mainly)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if(NOT ClangTidy)
  message(STATUS "clang-tidy not found")
else()
  include(ClangTidy)
  message(STATUS "clang-tidy found: ${ClangTidy}")
endif()

add_subdirectory(include)

# Tests and bench
if (BUILD_TESTS)
  add_subdirectory(tests)
  add_subdirectory(bench)
endif()

# Examples
if (BUILD_EXAMPLES)
  add_subdirectory(doc/src/examples)
endif()

if(ClangTidy)
  add_custom_target(tidy
    COMMAND ${ClangTidy}
    "-checks=-*,modernize-*,cppcoreguidelines-*,google-*,-google-readability-todo"
    "-warnings-as-errors=*" ${TIDY_SOURCES}
    COMMENT "Running static check..."
  )
endif()



message(STATUS "-------------------------------")
message(STATUS "++ Setup: ")
message(STATUS "System:        " ${CMAKE_SYSTEM})
message(STATUS "Build dir:     " ${CMAKE_BINARY_DIR})
message(STATUS "Build type:    " ${CMAKE_BUILD_TYPE})
message(STATUS "CXX:           " ${CMAKE_CXX_COMPILER})
message(STATUS "CXX_ID:        " ${CMAKE_CXX_COMPILER_ID})
message(STATUS "CXXFLAGS:      " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE_SUFFIX}})
message(STATUS "Verbose:       " ${CMAKE_VERBOSE_MAKEFILE})
message(STATUS "")
message(STATUS "++ Lib Info")
message(STATUS "ar:            " ${CMAKE_AR})
message(STATUS "ranlib:        " ${CMAKE_RANLIB})
message(STATUS "++ Options")
message(STATUS "Tests:         " ${BUILD_TESTS})
message(STATUS "-------------------------------")
