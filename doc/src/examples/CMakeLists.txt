add_executable(simple_example simple.cpp)
target_include_directories(simple_example PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(hooks_example hooks.cpp)
target_include_directories(hooks_example PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_custom_target(examples
  COMMAND
    ./hooks_example && ./simple_example
  DEPENDS
    hooks_example
    simple_example
)
