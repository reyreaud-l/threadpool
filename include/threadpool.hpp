#pragma once

#include <algorithm>
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
/*! \brief ThreadPool implement a multiple queues/multiple workers threadpool.
 *
 *  When created, the pool will start the workers(threads) immediatly. The
 *  threads will only terminate when the pool is destroyed.
 *
 *  This class implements a one queue per worker strategy to dispatch work.
 */
class ThreadPool
{
public:
  /*! \brief Constructs a ThreadPool.
   *
   *  The number of workers will be deduced from hardware.
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

  /* I don't like this implementation for hooks with a shared pointer. I don't
   * know why but it makes me feel uncomfortable.
   *
   * Our options are:
   * shared_ptr: easy solution. But do we really need shared ownership ? I don't
   * think it's necessary for such a simple interface.
   * unique_ptr: user probably wants to keep ownership of the hooks if it uses
   * them to store data. It would require a way to give back ownership to user
   * (ie give/take ala rust).
   * weak_ptr: requires the user to make a shared_ptr. Would clear the weak_ptr
   * when the shared_ptr is destroyed (which does not happen with raw pointer)
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

  /*! \brief Find the worker for which to dispatch the tasks
   *  \return The index in the worker array to which a task should be dispatch.
   */
  std::size_t get_dispatch_worker();

  /*! \brief Dispatch a task to a given worker
   *  \param idx Index of the worker to dispatch the work at
   *  \param task Task to dispatch into the worker
   */
  template <typename TaskType>
  void dispatch_work(const std::size_t idx, TaskType task);

  /*! \brief Inner worker class. Capture the ThreadPool when built.
   *
   *  The only job of this class is to run tasks. It will use the captured
   *  ThreadPool to interact with it.
   */
  struct Worker
  {
  public:
    /*! \brief Construct a worker.
     *  \param pool The ThreadPool the worker works for.
     */
    Worker(ThreadPool* pool);

    /*! \brief Poll task from the queue.
     *  \param nb_task Number of tasks to run and then exit. If 0 then run until
     *  the ThreadPool stops.
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

    std::atomic<std::size_t> queue_size;

  private:
    /*! \brief Captured ThreadPool that the worker works for.
     */
    ThreadPool* pool;
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
   */
  std::atomic<bool> stopped;

  /*! \brief Struct containing all hooks the threadpool will call.
   */
  std::shared_ptr<Hooks> hooks;
};

// ThreadPool implementation
// public:

inline ThreadPool::ThreadPool()
  : ThreadPool(std::thread::hardware_concurrency(),
               std::thread::hardware_concurrency())
{
}

inline ThreadPool::ThreadPool(std::size_t pool_size)
  : ThreadPool(pool_size, pool_size)
{
}

inline ThreadPool::ThreadPool(std::size_t pool_size, std::size_t max_pool_size)
  : waiting_threads(0)
  , working_threads(0)
  , pool_size(pool_size)
  , max_pool_size(max_pool_size)
  , stopped(false)
  , hooks(nullptr)
{
  start_pool();
}

inline ThreadPool::~ThreadPool()
{
  stop();
  clean();
}

template <typename Function, typename... Args>
auto ThreadPool::run(Function&& f, Args&&... args)
  -> std::future<typename std::result_of<Function(Args...)>::type>
{
  using task_return_type = typename std::result_of<Function(Args...)>::type;

  // Create a packaged task from the callable object to fetch its result
  // with get_future()
  auto inner_task = std::packaged_task<task_return_type()>(
    std::bind(std::forward<Function&&>(f), std::forward<Args&&...>(args)...));

  std::size_t idx = this->get_dispatch_worker();

  // TODO: If the pool can and should spawn more workers, spawn one for a
  // single task. The problem here is to handle synchronisation of modifying
  // the workers vector (when the threadpool is used by multiple thread. If
  // only one thread handles it, there is no problem.
  //
  // check_spawn_single_worker();

  auto result = inner_task.get_future();
  if (stopped)
  {
    return result;
  }
  dispatch_work(idx, std::move(inner_task));
  return result;
}

inline void ThreadPool::stop()
{
  stopped = true;
  for (auto& w : workers)
  {
    // This is needed in order for the worker to rerun the lambda in the
    // condition variable to check the condition (stopped) and exit the wait.
    // If this is not here, sometime a deadlock can occur as the thread will not
    // check the stopped boolean and remain waiting, while the main thread try
    // to join it and stops.
    {
      std::unique_lock<decltype(Worker::tasks_lock)> lock(w->tasks_lock);
    }
    w->cv_variable.notify_all();
  }
}

// ThreadPool implementation
// private:

inline void ThreadPool::start_pool()
{
  for (std::size_t i = 0; i < pool_size; i++)
  {
    add_worker();
  }
}

inline void ThreadPool::clean()
{
  for (auto& t : pool)
  {
    if (t.joinable())
    {
      CALL_HOOK_POOL(on_worker_die);
      t.join();
    }
  }
}

inline void ThreadPool::add_worker(std::size_t nb_task)
{
  auto w = std::unique_ptr<Worker>(new Worker(this));
  pool.emplace_back(std::ref(*w), nb_task);
  workers.push_back(std::move(w));
}

inline void ThreadPool::check_spawn_single_worker()
{
  // Check if we are allowed to spawn a worker
  if (max_pool_size > pool_size)
  {
    // Check if we have space to spawn a worker, and if it is valuable.
    if (working_threads.load() + waiting_threads.load() < max_pool_size)
    {
      CALL_HOOK_POOL(on_worker_add);
      add_worker(1);
    }
  }
}

template <typename TaskType>
inline void ThreadPool::dispatch_work(const std::size_t idx, TaskType task)
{
  workers[idx]->queue_size++;
  {
    std::lock_guard<decltype(Worker::tasks_lock)> lk(workers[idx]->tasks_lock);
    workers[idx]->tasks.emplace(std::move(task));
  }
  workers[idx]->cv_variable.notify_one();
}

inline std::size_t ThreadPool::get_dispatch_worker()
{
  // Random implementation
  static std::random_device seeder;
  static std::mt19937 engine(seeder());
  static std::uniform_int_distribution<int> dist(0, workers.size() - 1);
  return dist(engine);
  /*std::size_t idx = 0;
  std::size_t it = 0;
  std::size_t min = 0;
  std::for_each(std::begin(workers), std::end(workers),
                [&it, &idx, &min](const std::unique_ptr<Worker>& w) {
                  if (min < w->queue_size)
                  {
                    idx = it;
                    min = w->queue_size;
                  }
                  it++;
                });
  return idx;*/
}

// Worker implementation
// public:

inline ThreadPool::Worker::Worker(ThreadPool* pool)
  : pool(pool)
{
}

inline void ThreadPool::Worker::operator()(std::size_t nb_task)
{
  for (std::size_t i = 0; i != nb_task || nb_task == 0; i++)
  {
    // Thread is waiting
    pool->waiting_threads += 1;

    std::unique_lock<decltype(tasks_lock)> lock(tasks_lock);
    cv_variable.wait(lock, [&] { return pool->stopped || !tasks.empty(); });

    // Pool is stopped, discard task and exit
    if (pool->stopped)
      return;

    CALL_HOOK_WORKER(pre_task_hook);

    pool->waiting_threads -= 1;
    pool->working_threads += 1;

    // Fetch task
    std::packaged_task<void()> task = std::move(tasks.front());
    tasks.pop();
    queue_size--;

    // Release lock and exec task
    lock.unlock();
    task();

    // Task done
    CALL_HOOK_WORKER(post_task_hook);
    pool->working_threads -= 1;
  }
}

inline bool ThreadPool::is_stopped() const
{
  return stopped;
}

inline std::size_t ThreadPool::threads_available() const
{
  return waiting_threads.load();
}

inline std::size_t ThreadPool::threads_working() const
{
  return working_threads.load();
}

inline void ThreadPool::register_hooks(std::shared_ptr<Hooks> hooks)
{
  this->hooks = hooks;
}
} // namespace ThreadPool
