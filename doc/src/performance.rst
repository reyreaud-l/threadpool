Pool Performance
================

The pool performance can be measured using benchmarks.

Benchmark with neighbors work stealing enabled::

  Running ./benchbin
  Run on (4 X 3500 MHz CPU s)
  CPU Caches:
    L1 Data 32K (x4)
    L1 Instruction 32K (x4)
    L2 Unified 256K (x4)
    L3 Unified 6144K (x1)
  ----------------------------------------------------------------
  Benchmark                         Time           CPU Iterations
  ----------------------------------------------------------------
  bm_work_tasks/8                   0 ms          0 ms      12826
  bm_work_tasks/64                  0 ms          0 ms       7478
  bm_work_tasks/512                 1 ms          1 ms       1216
  bm_work_tasks/4096                5 ms          4 ms        157
  bm_work_tasks/32768              36 ms         36 ms         19
  bm_work_tasks/262144            302 ms        301 ms          2
  bm_work_tasks/2097152          2370 ms       2346 ms          1
  bm_blocking_tasks/8               0 ms          0 ms      10341
  bm_blocking_tasks/64              1 ms          0 ms       5379
  bm_blocking_tasks/512             8 ms          1 ms        600
  bm_blocking_tasks/4096           66 ms          4 ms        100
  bm_blocking_tasks/32768         525 ms         30 ms         10
  bm_blocking_tasks/262144       4160 ms        250 ms          1


Benchmark with work stealing across all workers enabled::

  Running ./benchbin
  Run on (4 X 3500 MHz CPU s)
  CPU Caches:
    L1 Data 32K (x4)
    L1 Instruction 32K (x4)
    L2 Unified 256K (x4)
    L3 Unified 6144K (x1)
  ----------------------------------------------------------------
  Benchmark                         Time           CPU Iterations
  ----------------------------------------------------------------
  bm_work_tasks/8                   0 ms          0 ms      12669
  bm_work_tasks/64                  0 ms          0 ms       7430
  bm_work_tasks/512                 1 ms          1 ms       1146
  bm_work_tasks/4096                5 ms          5 ms        153
  bm_work_tasks/32768              37 ms         36 ms         19
  bm_work_tasks/262144            311 ms        308 ms          2
  bm_work_tasks/2097152          2459 ms       2443 ms          1
  bm_blocking_tasks/8               0 ms          0 ms      11270
  bm_blocking_tasks/64              1 ms          0 ms       5836
  bm_blocking_tasks/512             8 ms          1 ms        600
  bm_blocking_tasks/4096           65 ms          4 ms        100
  bm_blocking_tasks/32768         516 ms         30 ms         10
  bm_blocking_tasks/262144       4125 ms        244 ms          1

Benchmark with work no work stealing::

  Running ./benchbin
  Run on (4 X 3500 MHz CPU s)
  CPU Caches:
    L1 Data 32K (x4)
    L1 Instruction 32K (x4)
    L2 Unified 256K (x4)
    L3 Unified 6144K (x1)
  ----------------------------------------------------------------
  Benchmark                         Time           CPU Iterations
  ----------------------------------------------------------------
  bm_work_tasks/8                   0 ms          0 ms      12989
  bm_work_tasks/64                  0 ms          0 ms       6765
  bm_work_tasks/512                 1 ms          1 ms       1181
  bm_work_tasks/4096                5 ms          5 ms        157
  bm_work_tasks/32768              36 ms         36 ms         19
  bm_work_tasks/262144            307 ms        305 ms          2
  bm_work_tasks/2097152          2407 ms       2393 ms          1
  bm_blocking_tasks/8               0 ms          0 ms       8766
  bm_blocking_tasks/64              1 ms          0 ms       4970
  bm_blocking_tasks/512             9 ms          1 ms        600
  bm_blocking_tasks/4096           67 ms          4 ms        100
  bm_blocking_tasks/32768         529 ms         30 ms         10
  bm_blocking_tasks/262144       4224 ms        242 ms          1
