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

#if __cplusplus >= 201500
// FIXME: should be next line but fails
// #define RETURN_TYPE(X) std::invoke_result_t<X>
// so instead use deprecated version
#define RETURN_TYPE(X) typename std::result_of<X>::type
#else
#define RETURN_TYPE(X) typename std::result_of<X>::type
#endif

class ThreadPool
{
public:
  /*! \brief Constructs a ThreadPool with pool_size workers.
   */
  ThreadPool(std::size_t pool_size);

  /*! \brief Constructs a ThreadPool with pool_size workers, and max_pool_size
   *  maximum workers.
   */
  ThreadPool(std::size_t pool_size, std::size_t max_pool_size);

  /*! \brief Stops the pool and clean all workers.
   *
   */
  ~ThreadPool();

  /*! \brief Run a task in the ThreadPool.
   *  \returns Return a future containing the result of the task.
   *
   *  When a task is ran in the ThreadPool, the callable object will be packaged
   *  in a packaged_task and put in the inner task_queue. A waiting worker will
   *  pick the task and execute it. If no workers are available, the task will
   *  remain in the queue until a worker picks it up.
   */
  template <typename Function, typename... Args>
  auto run(Function&& f, Args&&... args)
    -> std::future<RETURN_TYPE(Function(Args...))>;

  /*! \brief Stop the ThreadPool.
   *
   * A stopped ThreadPool will discard any task dispatched to it. All workers
   * will exit.
   */
  void stop();

  /*! \brief Returns if the ThreadPool is stopped.
   *
   */
  bool is_stop() const;

  /*! \brief Returns the number of threads currently waiting for a task.
   *
   * The number might be imprecise, as between the time the value is read and
   * returned, a thread might become unavailable.
   */
  std::size_t threads_available() const;

  /*! \brief Returns the number of threads currently executing a task.
   *
   * The number might be imprecise, as between the time the value is read and
   * returned, a thread might finish a task and become available.
   */
  std::size_t threads_working() const;

private:
  /*! \brief Inner worker class. Capture the ThreadPool when built.
   *
   *  The only job of this class is to run tasks. It will use the captured
   *  ThreadPool to interact with it.
   */
  struct Worker
  {
  public:
    /*! Construct a worker.
     * \param pool The ThreadPool the worker works for.
     */
    Worker(ThreadPool* pool);

    /*! \brief Poll task from the queue.
     *  \param nb_task Number of tasks to run and then exit. If 0 then run until
     *  the ThreadPool stops.
     */
    void operator()(std::size_t nb_task);

  private:
    /*! \brief Captured ThreadPool that the worker works for.
     */
    ThreadPool* _pool;
  };

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
};

inline ThreadPool::ThreadPool(std::size_t pool_size)
  : ThreadPool(pool_size, pool_size)

{
}

inline ThreadPool::ThreadPool(std::size_t pool_size, std::size_t max_pool_size)
  : _waiting_threads(0)
  , _working_threads(0)
  , _pool_size(pool_size)
  , _max_pool_size(max_pool_size)
{
  this->start_pool();
}

inline void ThreadPool::start_pool()
{
  for (std::size_t i = 0; i < this->_pool_size; i++)
    this->add_worker();
}

inline void ThreadPool::add_worker(std::size_t nb_task)
{
  // Instantiate a worker and emplace it in the pool.
  Worker w(this);
  _pool.emplace_back(w, nb_task);
}

template <typename Function, typename... Args>
auto ThreadPool::run(Function&& f, Args&&... args)
  -> std::future<RETURN_TYPE(Function(Args...))>
{
  using task_return_type = RETURN_TYPE(Function(Args...));

  // Create a packaged task from the callable object to fetch its result
  // with get_future()
  auto inner_task = std::packaged_task<task_return_type()>(
    std::bind(std::forward<Function&&>(f), std::forward<Args&&...>(args)...));

  auto result = inner_task.get_future();
  {
    this->check_spawn_single_worker();

    // If the pool can spawn more workers, spawn one for a single task
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

inline void ThreadPool::check_spawn_single_worker()
{
  // Check if we are allowed to spawn a worker
  if (this->_max_pool_size > this->_pool_size)
    // Check if we have space to spawn a worker, and if it is valuable.
    if (this->_working_threads.load() + this->_waiting_threads.load() <
        this->_max_pool_size)
      this->add_worker(1);
}

inline ThreadPool::Worker::Worker(ThreadPool* pool)
  : _pool(pool)
{
}

inline void ThreadPool::Worker::operator()(std::size_t nb_task)
{
  for (std::size_t i = 0; i != nb_task || nb_task == 0; i++)
  {
    // Thread is waiting
    _pool->_waiting_threads += 1;
    std::unique_lock<std::mutex> lock(_pool->_tasks_lock);
    _pool->_cv_variable.wait(
      lock, [&] { return _pool->_stop || !_pool->_tasks.empty(); });

    // Pool is stopped, discard task and exit
    if (_pool->_stop)
      return;

    _pool->_waiting_threads -= 1;
    _pool->_working_threads += 1;

    // Fetch task
    std::packaged_task<void()> task = std::move(_pool->_tasks.front());
    _pool->_tasks.pop();

    // Release lock and exec task
    lock.unlock();
    task();

    // Task done
    _pool->_working_threads -= 1;
  }
}

inline ThreadPool::~ThreadPool()
{
  this->stop();
  this->clean();
}

inline void ThreadPool::stop()
{
  std::lock_guard<std::mutex> lock(this->_tasks_lock);
  this->_stop = true;
  this->_cv_variable.notify_all();
}

inline void ThreadPool::clean()
{
  for (auto& t : _pool)
    if (t.joinable())
      t.join();
}

inline bool ThreadPool::is_stop() const
{
  return this->_stop;
}

inline std::size_t ThreadPool::threads_available() const
{
  return this->_waiting_threads.load();
}

inline std::size_t ThreadPool::threads_working() const
{
  return this->_working_threads.load();
}
