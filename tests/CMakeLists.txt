if (DOWNLOAD_DEPENDENCIES)
  include(GTest)
else()
  find_package(GTest REQUIRED)
endif()

add_executable(tests
  EXCLUDE_FROM_ALL
  main.cpp
  multiple_queue.cpp
  hooks.cpp
)

target_link_libraries(tests PRIVATE
  GTest::GTest
  Threads::Threads
)
target_include_directories(tests PRIVATE
  ${CMAKE_SOURCE_DIR}/tests
  ${CMAKE_SOURCE_DIR}/include
)

add_custom_target(check tests DEPENDS tests COMMENT "Running tests...")
