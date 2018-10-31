include(GBench)

add_executable(benchbin
  EXCLUDE_FROM_ALL
  main.cpp
)

target_link_libraries(benchbin PRIVATE
  Threads::Threads
  benchmark
)
target_include_directories(benchbin PRIVATE
  ${CMAKE_SOURCE_DIR}/bench
  ${CMAKE_SOURCE_DIR}/include
)

add_custom_target(bench benchbin DEPENDS benchbin COMMENT "Running benchs...")
