Pool Hooks
==========

Writing hooks
-------------
Hooks are written by creating a class or a struct which inherits from the class
:code:`ThreadPool::Hooks`. You can then override whichever hooks you need.

Tasks Hooks
~~~~~~~~~~~
.. doxygenfunction:: ThreadPool::Hooks::pre_task_hook()
.. doxygenfunction:: ThreadPool::Hooks::post_task_hook()

Workers Hooks
~~~~~~~~~~~~~
.. doxygenfunction:: ThreadPool::Hooks::on_worker_add()
.. doxygenfunction:: ThreadPool::Hooks::on_worker_die()

Registering hooks
-----------------
Hooks can be used to be notified when actions happens in the pool. The hooks are
registered in the pool using the function:

.. doxygenfunction:: ThreadPool::ThreadPool::register_hooks(std::shared_ptr<ThreadPool::Hooks>)

Example
-------

Here is a simple example to use hooks with the Pool.

.. literalinclude:: examples/hooks.cpp
  :language: c++
  :linenos:
