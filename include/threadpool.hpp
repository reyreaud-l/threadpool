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
// so instead use deprecated version in both cases.
#define RETURN_TYPE(X) typename std::result_of<X>::type
#else
#define RETURN_TYPE(X) typename std::result_of<X>::type
#endif

#define THREADPOOL_DEFAULT_HOOK(NAME)                                          \
  inline void ThreadPool::Hooks::NAME()                                        \
  {                                                                            \
    return;                                                                    \
  }

/*! \brief ThreadPool is a class representing a group of threads.
 *
 *  When created, the pool will start the workers(threads) immediatly. The
 *  threads will only terminate when the pool is destroyed.
 */
class ThreadPool
{
public:
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

  /*! \brief Inner class containing hooks the ThreadPool will call.
   *
   *  This class is used as an interface to allow user defined hooks to be
   *  registered.
   */
  struct Hooks
  {
    /*! \brief Default constructor
     */
    Hooks() = default;

    /*! \brief Default virtual destructor.
     *
     *  Make sure user defined destructor will be called.
     */
    virtual ~Hooks() = default;

    /*! \brief Hook called before picking a task.
     *
     *  This hook will be called by a worker before a task is executed. The
     *  worker will not have anything locked when calling the hook. The worker
     *  will call in a "working" state. That means that if the hook takes too
     *  long, the worker will hold on the task execution and not run it.
     *
     */
    virtual void pre_task_hook();

    /*! \brief Hook called after a task is done.
     *
     *  This hook will be called by a worker after a task is done. The worker
     *  will not have anything locked when calling the hook. The worker will
     *  call in a "working" state. That means that if the hook takes too long,
     *  the worker will hold and not pick a task until the hook is completed.
     */
    virtual void post_task_hook();
  };

  // I don't like this implementation with a shared pointer. I don't know why
  // but it makes me feel uncomfortable.
  //
  // Our options are:
  // shared_ptr: easy solution. But do we really need shared ownership ? I don't
  // think it's necessary for such a simple interface.
  // unique_ptr: user probably wants to keep ownership of the hooks if it uses
  // them to store data. It would require a way to give back ownership to user
  // (ie give/take ala rust).
  // weak_ptr: requires the user to make a shared_ptr. Would clear the weak_ptr
  // when the shared_ptr is destroyed (which does not happen with raw pointer)
  // reference: would be ideal, but need to be initialized to a dummy hook
  // interface at the beginning. Very easy to mess up ownership and acces and to
  // a stack use after scope error.
  //

  /*! \brief Register a ThreadPool::Hooks class.
   *  \param hooks The class to be registered
   *  \warning This function member makes a copy of the hooks given in
   *  parameter. If you wish to not make a copy, please use a shared_ptr and the
   *  other overloading to register a hook
   */
  void register_hooks(Hooks hooks);

  /*! \brief Register a ThreadPool::Hooks class.
   *  \param hooks The class to be registered
   *
   *  Prefer this method with a shared_ptr to the one without if you
   *  want to avoid copies and share ownership of the hook (to collect data for
   *  example and acces it in the main thread after).
   */
  void register_hooks(std::shared_ptr<Hooks> hooks);

private:
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

  /*! \brief Struct containing all hooks the threadpool will call.
   */
  std::shared_ptr<Hooks> _hooks;
};

// ThreadPool implementation
// public:

inline ThreadPool::ThreadPool(std::size_t pool_size)
  : ThreadPool(pool_size, pool_size)

{
}

inline ThreadPool::ThreadPool(std::size_t pool_size, std::size_t max_pool_size)
  : _waiting_threads(0)
  , _working_threads(0)
  , _pool_size(pool_size)
  , _max_pool_size(max_pool_size)
  , _hooks(nullptr)
{
  this->start_pool();
}

inline ThreadPool::~ThreadPool()
{
  this->stop();
  this->clean();
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

inline void ThreadPool::stop()
{
  // Should stop also call clean and stop the threads ?
  std::lock_guard<std::mutex> lock(this->_tasks_lock);
  this->_stop = true;
  this->_cv_variable.notify_all();
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

inline void ThreadPool::register_hooks(ThreadPool::Hooks hooks)
{
  _hooks = std::shared_ptr<ThreadPool::Hooks>(new ThreadPool::Hooks(hooks));
}

inline void ThreadPool::register_hooks(std::shared_ptr<ThreadPool::Hooks> hooks)
{
  _hooks = std::ref(hooks);
}

// ThreadPool implementation
// private:

inline void ThreadPool::start_pool()
{
  for (std::size_t i = 0; i < this->_pool_size; i++)
    this->add_worker();
}

inline void ThreadPool::clean()
{
  for (auto& t : _pool)
    if (t.joinable())
      t.join();
}

inline void ThreadPool::add_worker(std::size_t nb_task)
{
  // Instantiate a worker and emplace it in the pool.
  Worker w(this);
  _pool.emplace_back(w, nb_task);
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

// Worker implementation
// public:

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

    if (_pool->_hooks)
      _pool->_hooks->pre_task_hook();

    _pool->_waiting_threads -= 1;
    _pool->_working_threads += 1;

    // Fetch task
    std::packaged_task<void()> task = std::move(_pool->_tasks.front());
    _pool->_tasks.pop();

    // Release lock and exec task
    lock.unlock();
    task();

    if (_pool->_hooks)
      _pool->_hooks->post_task_hook();
    // Task done
    _pool->_working_threads -= 1;
  }
}

THREADPOOL_DEFAULT_HOOK(pre_task_hook)
THREADPOOL_DEFAULT_HOOK(post_task_hook)
