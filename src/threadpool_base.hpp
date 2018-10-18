#pragma once

#include <atomic>
#include <memory>

#include "hooks.hpp"

#define CALL_HOOK_WORKER(HOOK)                                                 \
  do                                                                           \
  {                                                                            \
    if (pool->hooks)                                                           \
      pool->hooks->HOOK();                                                     \
  } while (0)

#define CALL_HOOK_POOL(HOOK)                                                   \
  do                                                                           \
  {                                                                            \
    if (hooks)                                                                 \
      hooks->HOOK();                                                           \
  } while (0)

namespace ThreadPool
{
/*! \brief ThreadPoolBase contains whats common to all threadpool implemented in
 *   this library.
 *
 *   It mainly regroup getter/setter for common attributes. This class should
 *   never be instantiated explicitely by the user. It should only be used in
 *   the implementation of Threadpools.
 *
 */
class ThreadPoolBase
{
protected:
  /*! \brief Constructs a ThreadPoolBase.
   *  \param pool_size Number of threads to start.
   *  \param max_pool_size Maximum number of threads allowed, this will be used
   *  by the pool to extend the number of threads temporarily when all threads
   *  are used.
   *
   *  This constructor is protected because we don't want the user to be able to
   *  explicitely instantiate this class.
   */
  ThreadPoolBase(std::size_t pool_size, std::size_t max_pool_size)
    : waiting_threads(0)
    , working_threads(0)
    , pool_size(pool_size)
    , max_pool_size(max_pool_size)
    , hooks(nullptr)
  {
  }

public:
  virtual ~ThreadPoolBase()
  {
  }

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

  /*! \brief Check the state of the threadpool
   *  \returns True if the bool is stopped, false otherwise.
   */
  bool is_stopped() const;

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

protected:
  /*! \brief Number of waiting threads in the pool.
   */
  std::atomic<std::size_t> waiting_threads;

  /*! \brief Number of threads executing a task in the pool.
   */
  std::atomic<std::size_t> working_threads;

  /*! \brief Size of the pool.
   */
  const std::size_t pool_size;

  /*! \brief Max possible size of the pool.
   *
   *  This parameter is used to add additional threads if
   */
  const std::size_t max_pool_size;

  /*! \brief Boolean representing if the pool is stopped.
   *
   * Not an atomic as access to this boolean is always done under locking using
   * _tasks_lock_mutex.
   */
  bool stopped = false;

  /*! \brief Struct containing all hooks the threadpool will call.
   */
  std::shared_ptr<Hooks> hooks;
};
} // namespace ThreadPool
