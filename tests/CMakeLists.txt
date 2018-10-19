include(GTest)

set(TESTS_EXEC tests)
add_executable(${TESTS_EXEC}
  main.cpp
  single_queue.cpp
  hooks.cpp
)

target_link_libraries(${TESTS_EXEC} PRIVATE
  threadpool
  gtest_main
  Threads::Threads
)
target_include_directories(${TESTS_EXEC} PRIVATE ${CMAKE_SOURCE_DIR}/tests)

add_custom_target(check ${TESTS_EXEC} DEPENDS ${TESTS_EXEC} COMMENT "Running tests...")
