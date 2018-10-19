#pragma once

// DEBUG
#include <iostream>
// DEBUG

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <random>
#include <thread>
#include <type_traits>
#include <vector>

#include "hooks.hpp"
#include "threadpool_base.hpp"

namespace ThreadPool
{
/*! \brief MultipleQueue is a class implementating the ThreadPool interface.
 *
 *  When created, the pool will start the workers(threads) immediatly. The
 *  threads will only terminate when the pool is destroyed.
 *
 *  This class implements a one queue per worker strategy to dispatch work.
 */
class MultipleQueue : public ThreadPoolBase
{
public:
  /*! \brief Constructs a MultipleQueue.
   *  The pool size will be detucted from number of threads available on the
   *  machine/
   */
  MultipleQueue();

  /*! \brief Constructs a MultipleQueue.
   *  \param pool_size Number of threads to start.
   */
  MultipleQueue(std::size_t pool_size);

  /*! \brief Constructs a MultipleQueue.
   *  \param pool_size Number of threads to start.
   *  \param max_pool_size Maximum number of threads allowed, this will be used
   *  by the pool to extend the number of threads temporarily when all threads
   *  are used.
   */
  MultipleQueue(std::size_t pool_size, std::size_t max_pool_size);

  /*! \brief Stops the pool and clean all workers.
   */
  ~MultipleQueue();

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
    -> std::future<typename std::result_of<Function(Args...)>::type>;

  /*! \brief Stop the SingleQueue.
   *
   * A stopped SingleQueue will discard any task dispatched to it. All workers
   * will discard new tasks, but the threads will not exit.
   */
  void stop();

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

  /*! \brief Inner worker class. Capture the MultipleQueue when built.
   *
   *  The only job of this class is to run tasks. It will use the captured
   *  MultipleQueue to interact with it.
   */
  struct Worker
  {
  public:
    /*! \brief Construct a worker.
     *  \param pool The MultipleQueue the worker works for.
     */
    Worker(MultipleQueue* pool);

    /*! \brief Poll task from the queue.
     *  \param nb_task Number of tasks to run and then exit. If 0 then run until
     *  the MultipleQueue stops.
     */
    void operator()(std::size_t nb_task);

    /*! \brief The task queue.
     *
     *  Access to this task queue should **always** be done while locking using
     *  tasks_lock mutex.
     */
    std::queue<std::packaged_task<void()>> tasks;

    /*! \brief Mutex regulating acces to _tasks.
     */
    std::mutex tasks_lock;

    /*! \brief Condition variable used to wake the worker for when a task is
     *  available.
     */
    std::condition_variable cv_variable;

  private:
    /*! \brief Captured MultipleQueue that the worker works for.
     */
    MultipleQueue* pool;
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

  /*! \brief Vector of thread, the actual thread pool.
   *
   *  Emplacing in this vector construct and launch a thread.
   */
  std::vector<std::thread> pool;

  /*! \brief Vector of the workers in the pool.
   */
  std::vector<std::unique_ptr<Worker>> workers;
};

template <typename Function, typename... Args>
auto MultipleQueue::run(Function&& f, Args&&... args)
  -> std::future<typename std::result_of<Function(Args...)>::type>
{
  using task_return_type = typename std::result_of<Function(Args...)>::type;

  // Create a packaged task from the callable object to fetch its result
  // with get_future()
  auto inner_task = std::packaged_task<task_return_type()>(
    std::bind(std::forward<Function&&>(f), std::forward<Args&&...>(args)...));

  // Random init
  static std::random_device seeder;
  static std::mt19937 engine(seeder());
  static std::uniform_int_distribution<int> dist(0, workers.size() - 1);
  std::size_t idx = dist(engine);

  // TODO: If the pool can and should spawn more workers, spawn one for a
  // single task. The problem here is to handle synchronisation of modifying
  // the workers vector (when the threadpool is used by multiple thread. If
  // only one thread handles it, there is no problem.
  //
  // check_spawn_single_worker();

  auto result = inner_task.get_future();
  {
    if (stopped)
      return result;
    std::lock_guard<decltype(Worker::tasks_lock)> lk(workers[idx]->tasks_lock);
    workers[idx]->tasks.emplace(std::move(inner_task));
  }

  workers[idx]->cv_variable.notify_one();
  return result;
}
} // namespace ThreadPool
