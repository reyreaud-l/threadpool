include(GTest)

add_executable(tests
  EXCLUDE_FROM_ALL
  main.cpp
  multiple_queue.cpp
  hooks.cpp
)

target_link_libraries(tests PRIVATE
  gtest_main
  Threads::Threads
)
target_include_directories(tests PRIVATE
  ${CMAKE_SOURCE_DIR}/tests
  ${CMAKE_SOURCE_DIR}/include
)

add_custom_target(check tests DEPENDS tests COMMENT "Running tests...")
