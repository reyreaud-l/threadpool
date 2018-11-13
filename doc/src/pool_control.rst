Pool Control
============

Pool size
---------

You can control the threadpool size when instantiating it with the constructors.
Here are the different constructors you can use:

.. doxygenfunction:: ThreadPool::ThreadPool::ThreadPool(std::size_t)
.. doxygenfunction:: ThreadPool::ThreadPool::ThreadPool(std::size_t, std::shared_ptr<Hooks>)

The :code:`pool_size` parameter will determine the number of workers(threads)
the pool will start when instantiating.

Stopping the pool
-----------------

You can stop the pool with:

.. doxygenfunction:: ThreadPool::ThreadPool::stop()

You can check if the pool is stopped with:

.. doxygenfunction:: ThreadPool::ThreadPool::is_stopped()

Checking the pool
-----------------

You can check the current state of the workers with:

.. doxygenfunction:: ThreadPool::ThreadPool::threads_available()
.. doxygenfunction:: ThreadPool::ThreadPool::threads_working()
