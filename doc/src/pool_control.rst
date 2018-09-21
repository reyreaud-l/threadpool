Pool Control
============

Pool size
---------

You can control the threadpool size when instantiating it with the constructors.
Here are the different constructors you can use:

.. doxygenfunction:: ThreadPool::ThreadPool(std::size_t)
.. doxygenfunction:: ThreadPool::ThreadPool(std::size_t, std::size_t)

The :code:`pool_size` parameter will determine the number of workers(threads)
the pool will start when instantiating. The :code:`max_pool_size` is used by the
pool to add more workers when all workers are working and a task comes in.

For the first constructor, the :code:`max_pool_size` will be the
:code:`pool_size` given as argument.

Stopping the pool
-----------------

You can stop the pool with:

.. doxygenfunction:: ThreadPool::stop()

You can check if the pool is stopped with:

.. doxygenfunction:: ThreadPool::is_stop()

Checking the pool
-----------------

You can check the current state of the workers with:

.. doxygenfunction:: ThreadPool::threads_available()
.. doxygenfunction:: ThreadPool::threads_working()
