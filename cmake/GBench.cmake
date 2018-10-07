find_package(Threads REQUIRED)
file(MAKE_DIRECTORY benchmark)

include(ExternalProject)
set(EXCLUDE_FROM_ALL_OPT "")
set(EXCLUDE_FROM_ALL_VALUE "")
if (${CMAKE_VERSION} VERSION_GREATER "3.0.99")
    set(EXCLUDE_FROM_ALL_OPT "EXCLUDE_FROM_ALL")
    set(EXCLUDE_FROM_ALL_VALUE "ON")
endif()
ExternalProject_Add(googlebench
    ${EXCLUDE_FROM_ALL_OPT} ${EXCLUDE_FROM_ALL_VALUE}
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG master
    PREFIX "${CMAKE_BINARY_DIR}/benchmark"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/benchmark"
    CMAKE_CACHE_ARGS
      -DCMAKE_BUILD_TYPE:STRING=RELEASE
      -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
      -DCMAKE_INSTALL_LIBDIR:PATH=<INSTALL_DIR>/lib
      -DBENCHMARK_ENABLE_GTEST_TESTS:BOOL=OFF
      -DBENCHMARK_ENABLE_TESTING:BOOL=OFF
    )

ExternalProject_Get_Property(googlebench install_dir)
set(BENCHMARK_INCLUDE_DIRS ${install_dir}/include)
file(MAKE_DIRECTORY ${BENCHMARK_INCLUDE_DIRS})

set(LIB_SUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(LIB_PREFIX "${CMAKE_STATIC_LIBRARY_PREFIX}")
if("${GTEST_BUILD_TYPE}" STREQUAL "DEBUG")
  set(LIB_SUFFIX "d${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()

foreach(LIB benchmark)
  add_library(${LIB} UNKNOWN IMPORTED)
  set_target_properties(${LIB} PROPERTIES
    IMPORTED_LOCATION ${install_dir}/lib/${LIB_PREFIX}${LIB}${LIB_SUFFIX}
    INTERFACE_INCLUDE_DIRECTORIES ${BENCHMARK_INCLUDE_DIRS}
    INTERFACE_LINK_LIBRARIES "benchmark"
  )
  add_dependencies(${LIB} googlebench)
endforeach()
