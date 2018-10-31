Pool Performance
================

In the future, the goal is to have multiple strategies to have a different
dispatch mechanism for the threadpool. For now a single strategy is used.

A benchmark can help measure the performance of the pool:

.. code-block:: none

   Running ./benchbin
   Run on (4 X 3500 MHz CPU s)
   CPU Caches:
     L1 Data 32K (x4)
     L1 Instruction 32K (x4)
     L2 Unified 256K (x4)
     L3 Unified 6144K (x1)
   -----------------------------------------------------------------------------
   Benchmark                                      Time           CPU Iterations
   -----------------------------------------------------------------------------
   BM_ThreadPool_NoBlockingTasks/8                0 ms          0 ms      12931
   BM_ThreadPool_NoBlockingTasks/64               0 ms          0 ms       6326
   BM_ThreadPool_NoBlockingTasks/512              1 ms          1 ms       1162
   BM_ThreadPool_NoBlockingTasks/4096             4 ms          4 ms        161
   BM_ThreadPool_NoBlockingTasks/32768           35 ms         34 ms         20
   BM_ThreadPool_NoBlockingTasks/262144         285 ms        281 ms          2
   BM_ThreadPool_NoBlockingTasks/2097152       2305 ms       2281 ms          1
   BM_ThreadPool_NoBlockingTasks/8388608       9153 ms       9037 ms          1
