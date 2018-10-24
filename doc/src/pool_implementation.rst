Pool Implementation
===================

The library include multiple implementation of a ThreadPool.
For now here are the implemented threadpools:

- ThreadPool::SingleQueue (single queue, multiple workers) <- Default
- ThreadPool::MultipleQueue (multiple queues, multiple workers)

Selecting the implementation
----------------------------

You can select which one you desire by precising it's type in the template
parameter (or leave it empty to have a default one).

Here the selected implementation will be the default one (:code:`SingleQueue`):

.. code-block:: c++
  :linenos:

  ...
  ThreadPool::ThreadPool<> pool;
  ...

And here the :code:`MutlipleQueue` implementation will be selected.

.. code-block:: c++
  :linenos:

  ...
  ThreadPool::ThreadPool<ThreadPool::MultipleQueue> pool;
  ...

Due to the fact that the implementation is selected with a template, the
implementation cannot be changed at runtime. This is due to the fact that the
:code:`ThreadPool` class cannot be an interface using virtual overloading as it
uses template method (the :code:`run` method in particular).

Performance
-----------

Performance comparison can be found by running the benchmark :code:`make bench`.
Here is an example result, on my laptop Linux x86_64 with an Intel i5-7300HQ @2.50 GHz
in release mode.

.. code-block:: none

  Running ./benchbin
   Run on (4 X 3500 MHz CPU s)
   CPU Caches:
     L1 Data 32K (x4)
     L1 Instruction 32K (x4)
     L2 Unified 256K (x4)
     L3 Unified 6144K (x1)
   ------------------------------------------------------------------------------------------
   Benchmark                                                   Time           CPU Iterations
   ------------------------------------------------------------------------------------------
   BM_ThreadPool_SingleQueue_NoBlockingTasks/8                 0 ms          0 ms      12109
   BM_ThreadPool_SingleQueue_NoBlockingTasks/64                0 ms          0 ms       3912
   BM_ThreadPool_SingleQueue_NoBlockingTasks/512               1 ms          1 ms        623
   BM_ThreadPool_SingleQueue_NoBlockingTasks/4096              9 ms          8 ms         89
   BM_ThreadPool_SingleQueue_NoBlockingTasks/32768            75 ms         63 ms         12
   BM_ThreadPool_SingleQueue_NoBlockingTasks/131072          284 ms        237 ms          3
   BM_ThreadPool_MultipleQueue_NoBlockingTasks/8               0 ms          0 ms      12529
   BM_ThreadPool_MultipleQueue_NoBlockingTasks/64              0 ms          0 ms       6445
   BM_ThreadPool_MultipleQueue_NoBlockingTasks/512             1 ms          1 ms       1240
   BM_ThreadPool_MultipleQueue_NoBlockingTasks/4096            5 ms          4 ms        181
   BM_ThreadPool_MultipleQueue_NoBlockingTasks/32768          32 ms         27 ms         27
   BM_ThreadPool_MultipleQueue_NoBlockingTasks/131072        126 ms        115 ms          7
