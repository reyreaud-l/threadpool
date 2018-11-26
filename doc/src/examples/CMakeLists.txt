add_executable(simple_example EXCLUDE_FROM_ALL simple.cpp)
target_include_directories(simple_example PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(simple_example PRIVATE Threads::Threads)

add_executable(hooks_example EXCLUDE_FROM_ALL hooks.cpp)
target_include_directories(hooks_example PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(hooks_example PRIVATE Threads::Threads)

add_custom_target(examples
  COMMAND
    ./hooks_example && ./simple_example
  DEPENDS
    hooks_example
    simple_example
)
