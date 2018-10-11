Pool Implementation
===================

The library include multiple implementation of a ThreadPool.
For now here are the implemented threadpools:
- Default: ThreadPool::SQMW (single queue, multiple workers)

Selecting the implementation
----------------------------

You can select which one you desire by precising it's type in the template
parameter (or leave it empty to have a default one).

Here the selected implementation will be the default one:

.. code-block:: c++
  :linenos:

  ...
  ThreadPool::ThreadPool<> pool;
  ...

And here the :code:`SQMW` implementation will be selected.

.. code-block:: c++
  :linenos:

  ...
  ThreadPool::ThreadPool<ThreadPool::SQMW> pool;
  ...
