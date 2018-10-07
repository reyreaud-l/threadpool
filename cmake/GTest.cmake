include(split_list)

find_package(Threads REQUIRED)
file(MAKE_DIRECTORY googletest)

include(ExternalProject)
set(EXCLUDE_FROM_ALL_OPT "")
set(EXCLUDE_FROM_ALL_VALUE "")
set(GTEST_FLAGS "")
list(APPEND GTEST_FLAGS -std=c++11)
split_list(GTEST_FLAGS)
if (${CMAKE_VERSION} VERSION_GREATER "3.0.99")
    set(EXCLUDE_FROM_ALL_OPT "EXCLUDE_FROM_ALL")
    set(EXCLUDE_FROM_ALL_VALUE "ON")
endif()
ExternalProject_Add(googletest
    ${EXCLUDE_FROM_ALL_OPT} ${EXCLUDE_FROM_ALL_VALUE}
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG master
    PREFIX "${CMAKE_BINARY_DIR}/googletest"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/googletest"
    CMAKE_CACHE_ARGS
      -DCMAKE_BUILD_TYPE:STRING=RELEASE
      -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
      -DCMAKE_INSTALL_LIBDIR:PATH=<INSTALL_DIR>/lib
      -DCMAKE_CXX_FLAGS:STRING=${GTEST_FLAGS}
    )

ExternalProject_Get_Property(googletest install_dir)
set(GTEST_INCLUDE_DIRS ${install_dir}/include)
file(MAKE_DIRECTORY ${GTEST_INCLUDE_DIRS})

set(LIB_SUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(LIB_PREFIX "${CMAKE_STATIC_LIBRARY_PREFIX}")
if("${GTEST_BUILD_TYPE}" STREQUAL "DEBUG")
  set(LIB_SUFFIX "d${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()

# Use gmock_main instead of gtest_main because it initializes gtest as well.
# Note: The libraries are listed in reverse order of their dependancies.
foreach(LIB gtest gtest_main)
  add_library(${LIB} UNKNOWN IMPORTED)
  set_target_properties(${LIB} PROPERTIES
    IMPORTED_LOCATION ${install_dir}/lib/${LIB_PREFIX}${LIB}${LIB_SUFFIX}
    INTERFACE_INCLUDE_DIRECTORIES ${GTEST_INCLUDE_DIRS}
    INTERFACE_LINK_LIBRARIES "gtest"
  )
  add_dependencies(${LIB} googletest)
endforeach()
