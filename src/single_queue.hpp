#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

#include "hooks.hpp"

#if __cplusplus >= 201500
// FIXME: should be next line but fails
// #define RETURN_TYPE(X) std::invoke_result_t<X>
// so instead use deprecated version in both cases.
#define RETURN_TYPE(X) typename std::result_of<X>::type
#else
#define RETURN_TYPE(X) typename std::result_of<X>::type
#endif

#define CALL_HOOK_WORKER(HOOK)                                                 \
  do                                                                           \
  {                                                                            \
    if (_pool->_hooks)                                                         \
      _pool->_hooks->HOOK();                                                   \
  } while (0)

#define CALL_HOOK_POOL(HOOK)                                                   \
  do                                                                           \
  {                                                                            \
    if (_hooks)                                                                \
      _hooks->HOOK();                                                          \
  } while (0)

namespace ThreadPool
{
/*! \brief SQWM is a class implementating the ThreadPool interface.
 *
 *  When created, the pool will start the workers(threads) immediatly. The
 *  threads will only terminate when the pool is destroyed.
 *
 *  This class implements a single queue, multiple worker strategy to dispatch
 *  work.
 */
class SingleQueue
{
public:
  /*! \brief Constructs a SingleQueue.
   *  The pool size will be detucted from number of threads available on the
   *  machine/
   */
  SingleQueue();

  /*! \brief Constructs a SingleQueue.
   *  \param pool_size Number of threads to start.
   */
  SingleQueue(std::size_t pool_size);

  /*! \brief Constructs a SingleQueue.
   *  \param pool_size Number of threads to start.
   *  \param max_pool_size Maximum number of threads allowed, this will be used
   *  by the pool to extend the number of threads temporarily when all threads
   *  are used.
   */
  SingleQueue(std::size_t pool_size, std::size_t max_pool_size);

  /*! \brief Stops the pool and clean all workers.
   */
  ~SingleQueue();

  /*! \brief Run a task in the SingleQueue.
   *  \returns Returns a future containing the result of the task.
   *
   *  When a task is ran in the SingleQueue, the callable object will be
   * packaged in a packaged_task and put in the inner task_queue. A waiting
   * worker will pick the task and execute it. If no workers are available, the
   * task will remain in the queue until a worker picks it up.
   */
  template <typename Function, typename... Args>
  auto run(Function&& f, Args&&... args)
    -> std::future<RETURN_TYPE(Function(Args...))>;

  /*! \brief Stop the SingleQueue.
   *
   * A stopped SingleQueue will discard any task dispatched to it. All workers
   * will discard new tasks, but the threads will not exit.
   */
  void stop();

  /*! \brief Check the state of the threadpool
   *  \returns True if the bool is stopped, false otherwise.
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

  /*! \brief Register a SingleQueue::Hooks class.
   *  \param hooks The class to be registered
   */
  void register_hooks(std::shared_ptr<Hooks> hooks);

private:
  /*! \brief Starts the pool when the pool is constructed.
   *
   *  It will starts _pool_size threads.
   */
  void start_pool();

  /*! \brief Joins all threads in the pool.
   *
   *  Should only be called from destructor.
   */
  void clean();

  /*! \brief Inner worker class. Capture the SingleQueue when built.
   *
   *  The only job of this class is to run tasks. It will use the captured
   *  SingleQueue to interact with it.
   */
  struct Worker
  {
  public:
    /*! \brief Construct a worker.
     *  \param pool The SingleQueue the worker works for.
     */
    Worker(SingleQueue* pool);

    /*! \brief Poll task from the queue.
     *  \param nb_task Number of tasks to run and then exit. If 0 then run until
     *  the SingleQueue stops.
     */
    void operator()(std::size_t nb_task);

  private:
    /*! \brief Captured SingleQueue that the worker works for.
     */
    SingleQueue* _pool;
  };

  /*! \brief Start a worker for a nb_task.
   *  \param nb_task Number of tasks to execute.
   *
   *  If nb_task is 0 (default arg) the worker will remain alive until
   *  threadpool is stopped.
   */
  void add_worker(std::size_t nb_task = 0);

  /*! \brief Check if the pool can spawn more workers, and spawn one for a
   *  single task
   *
   *  It will check the current number of spawned threads and if it can spawn
   *  or not a new thread. If a thread can be spawned, one is created for a
   *  single task.
   */
  void check_spawn_single_worker();

  /*! \brief Number of waiting threads in the pool.
   */
  std::atomic<std::size_t> _waiting_threads;

  /*! \brief Number of threads executing a task in the pool.
   */
  std::atomic<std::size_t> _working_threads;

  /*! \brief Size of the pool.
   */
  const std::size_t _pool_size;

  /*! \brief Max possible size of the pool.
   *
   *  This parameter is used to add additional threads if
   */
  const std::size_t _max_pool_size;

  /*! \brief Boolean representing if the pool is stopped.
   *
   * Not an atomic as access to this boolean is always done under locking using
   * _tasks_lock_mutex.
   */
  bool _stop = false;

  /*! \brief Vector of thread, the actual thread pool.
   *
   *  Emplacing in this vector construct and launch a thread.
   */
  std::vector<std::thread> _pool;

  /*! \brief The task queue.
   *
   *  Access to this task queue should **always** be done while locking using
   *  _tasks_lock mutex.
   */
  std::queue<std::packaged_task<void()>> _tasks;

  /*! \brief Mutex regulating acces to _tasks.
   */
  std::mutex _tasks_lock;

  /*! \brief Condition variable used in workers to wait for an available task.
   */
  std::condition_variable _cv_variable;

  /*! \brief Struct containing all hooks the threadpool will call.
   */
  std::shared_ptr<Hooks> _hooks;
};

template <typename Function, typename... Args>
auto SingleQueue::run(Function&& f, Args&&... args)
  -> std::future<RETURN_TYPE(Function(Args...))>
{
  using task_return_type = RETURN_TYPE(Function(Args...));

  // Create a packaged task from the callable object to fetch its result
  // with get_future()
  auto inner_task = std::packaged_task<task_return_type()>(
    std::bind(std::forward<Function&&>(f), std::forward<Args&&...>(args)...));

  auto result = inner_task.get_future();
  {
    // If the pool can spawn more workers, spawn one for a single task
    this->check_spawn_single_worker();

    // Lock the queue and emplace move the ownership of the task inside
    std::lock_guard<std::mutex> lock(this->_tasks_lock);

    if (this->_stop)
      return result;
    this->_tasks.emplace(std::move(inner_task));
  }
  // Notify one worker that a task is available
  _cv_variable.notify_one();
  return result;
}
} // namespace ThreadPool
