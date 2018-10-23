include(GBench)

add_executable(benchbin main.cpp)

target_link_libraries(benchbin PRIVATE
  threadpool
  Threads::Threads
  benchmark
)
target_include_directories(benchbin PRIVATE ${CMAKE_SOURCE_DIR}/bench)

add_custom_target(bench benchbin DEPENDS benchbin COMMENT "Running benchs...")
