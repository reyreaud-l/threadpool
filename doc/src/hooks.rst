Pool Hooks
==========

Writing hooks
-------------
Hooks are written by creating a class or a struct which inherits from the class
:code:`ThreadPool::Hooks`. You can then override whichever hooks you need.

Here are the hooks you can override.

.. doxygenstruct:: ThreadPool::Hooks
   :members:
   :protected-members:
   :no-link:

Registering hooks
-----------------
Hooks can be used to be notified when actions happens in the pool. The hooks are
registered in the pool using the functions:

.. doxygenfunction:: ThreadPool::register_hooks(ThreadPool::Hooks)
.. doxygenfunction:: ThreadPool::register_hooks(std::shared_ptr<ThreadPool::Hooks>)

Example
-------

Here is a simple example to use hooks with the Pool.

.. literalinclude:: examples/hooks.cpp
  :language: c++
  :linenos:
