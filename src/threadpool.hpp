#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

#include "hooks.hpp"
#include "sqmw.hpp"

#if __cplusplus >= 201500
// FIXME: should be next line but fails
// #define RETURN_TYPE(X) std::invoke_result_t<X>
// so instead use deprecated version in both cases.
#define RETURN_TYPE(X) typename std::result_of<X>::type
#else
#define RETURN_TYPE(X) typename std::result_of<X>::type
#endif

/*! \brief ThreadPool is a class representing a group of threads.
 *
 *  When created, the pool will start the workers(threads) immediatly. The
 *  threads will only terminate when the pool is destroyed.
 */
namespace ThreadPool
{
template <typename Impl = SQMW>
class ThreadPool
{
public:
  /*! \brief Constructs a ThreadPool.
   *  The pool size will be detucted from number of threads available on the
   *  machine/
   */
  ThreadPool();

  /*! \brief Constructs a ThreadPool.
   *  \param pool_size Number of threads to start.
   */
  ThreadPool(std::size_t pool_size);

  /*! \brief Constructs a ThreadPool.
   *  \param pool_size Number of threads to start.
   *  \param max_pool_size Maximum number of threads allowed, this will be used
   *  by the pool to extend the number of threads temporarily when all threads
   *  are used.
   */
  ThreadPool(std::size_t pool_size, std::size_t max_pool_size);

  /*! \brief Stops the pool and clean all workers.
   */
  ~ThreadPool();

  /*! \brief Run a task in the ThreadPool.
   *  \returns Returns a future containing the result of the task.
   *
   *  When a task is ran in the ThreadPool, the callable object will be packaged
   *  in a packaged_task and then it will be processed. The process will depend
   *  on the selected implementation.
   */
  template <typename Function, typename... Args>
  auto run(Function&& f, Args&&... args)
    -> std::future<RETURN_TYPE(Function(Args...))>;

  /*! \brief Stop the ThreadPool.
   *
   * A stopped ThreadPool will discard any task dispatched to it. All workers
   * will discard new tasks, but the threads will not exit.
   */
  void stop();

  /*! \brief Check the state of the threadpool
   *  \returns True if the pool is stopped, false otherwise.
   */
  bool is_stop() const;

  /*! \brief Check on the number of threads not currently working.
   *  \returns The number of threads currently waiting for a task.
   *
   * The number might be imprecise, as between the time the value is read and
   * returned, a thread might become unavailable.
   */
  std::size_t threads_available() const;

  /*! \brief Check on the number of threads currently working.
   *  \returns The number of threads currently working.
   *
   * The number might be imprecise, as between the time the value is read and
   * returned, a thread might finish a task and become available.
   */
  std::size_t threads_working() const;

  /* I don't like this implementation with a shared pointer. I don't know why
     but it makes me feel uncomfortable.

     Our options are:
     shared_ptr: easy solution. But do we really need shared ownership ? I don't
     think it's necessary for such a simple interface.
     unique_ptr: user probably wants to keep ownership of the hooks if it uses
     them to store data. It would require a way to give back ownership to user
     (ie give/take ala rust).
     weak_ptr: requires the user to make a shared_ptr. Would clear the weak_ptr
     when the shared_ptr is destroyed (which does not happen with raw pointer)
  */

  /*! \brief Register a ThreadPool::Hooks class.
   *  \param hooks The class to be registered
   */
  void register_hooks(std::shared_ptr<Hooks> hooks);

private:
  /*! \brief This member is the actual threadpool implementation.
   *
   *  All calls are forwared to impl. This composition is required as virtual
   *  and templates don't work together. Therefore to have an "interface" kind
   *  of stuff with this class, inheritance was ruled out. This policy based
   *  design is usefull to have a class act as an interface.
   */
  std::unique_ptr<Impl> impl;
};

// ThreadPool implementation
// public:

template <typename Impl>
inline ThreadPool<Impl>::ThreadPool()
  : ThreadPool(std::thread::hardware_concurrency(),
               std::thread::hardware_concurrency())
{
}

template <typename Impl>
inline ThreadPool<Impl>::ThreadPool(std::size_t pool_size)
  : ThreadPool(pool_size, pool_size)

{
}

template <typename Impl>
inline ThreadPool<Impl>::ThreadPool(std::size_t pool_size,
                                    std::size_t max_pool_size)
{
  impl = std::unique_ptr<Impl>(new Impl(pool_size, max_pool_size));
}

template <typename Impl>
inline ThreadPool<Impl>::~ThreadPool()
{
  this->impl->stop();
}

template <typename Impl>
template <typename Function, typename... Args>
auto ThreadPool<Impl>::run(Function&& f, Args&&... args)
  -> std::future<RETURN_TYPE(Function(Args...))>
{
  return this->impl->run(std::forward<Function&&>(f),
                         std::forward<Args&&>(args)...);
}

template <typename Impl>
inline void ThreadPool<Impl>::stop()
{
  this->impl->stop();
}

template <typename Impl>
inline bool ThreadPool<Impl>::is_stop() const
{
  return this->impl->is_stop();
}

template <typename Impl>
inline std::size_t ThreadPool<Impl>::threads_available() const
{
  return this->impl->threads_available();
}

template <typename Impl>
inline std::size_t ThreadPool<Impl>::threads_working() const
{
  return this->impl->threads_working();
}

template <typename Impl>
inline void ThreadPool<Impl>::register_hooks(std::shared_ptr<Hooks> hooks)
{
  this->impl->register_hooks(hooks);
}
} // namespace ThreadPool
