include(GBench)

set(BENCH_EXEC benchbin)
add_executable(${BENCH_EXEC}
  main.cpp
)

target_link_libraries(${BENCH_EXEC} PRIVATE
  Threads::Threads
  benchmark
)
target_include_directories(${BENCH_EXEC} PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/bench
)

add_custom_target(bench ${BENCH_EXEC} DEPENDS ${BENCH_EXEC} COMMENT "Running benchs...")
