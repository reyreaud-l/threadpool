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
  ThreadPool(std::size_t pool_size);
  ThreadPool(std::size_t pool_size, std::size_t max_pool_size);
  ~ThreadPool();

  // Dispatch a task in the thread pool
  template <typename Function, typename... Args>
  auto run(Function&& f, Args&&... args)
    -> std::future<RETURN_TYPE(Function(Args...))>;

  // Stop the thread pool and notify all workers to stop
  void stop();

  // Getters to fetch current pool status
  bool is_stop() const;
  std::size_t threads_available() const;
  std::size_t threads_working() const;

private:
  struct Worker
  {
  public:
    Worker(ThreadPool* pool);

    // Run nb_task tasks. If nb_task is zero, run until pool stops.
    void operator()(std::size_t nb_task);

  private:
    ThreadPool* _pool;
  };

  void start_pool();
  void clean();

  void add_worker();
  void add_worker_tasks(std::size_t);

  std::atomic<std::size_t> _waiting_threads;
  std::atomic<std::size_t> _working_threads;
  const std::size_t _pool_size;
  const std::size_t _max_pool_size;

  std::vector<std::thread> _pool;
  std::queue<std::packaged_task<void()>> _tasks;
  std::condition_variable _cv_variable;
  bool _stop = false;
  std::mutex _tasks_lock;
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

inline void ThreadPool::add_worker()
{
  this->add_worker_tasks(0);
}

inline void ThreadPool::add_worker_tasks(std::size_t nb_task)
{
  Worker w(this);
  _pool.emplace_back(w, nb_task);
}

template <typename Function, typename... Args>
auto ThreadPool::run(Function&& f, Args&&... args)
  -> std::future<RETURN_TYPE(Function(Args...))>
{
  using task_return_type = RETURN_TYPE(Function(Args...));

  auto inner_task = std::packaged_task<task_return_type()>(
    std::bind(std::forward<Function>(f), std::forward<Args...>(args)...));

  auto result = inner_task.get_future();
  {
    std::lock_guard<std::mutex> lock(this->_tasks_lock);
    if (this->_stop)
      return result;
    this->_tasks.emplace(std::move(inner_task));
  }
  _cv_variable.notify_one();
  return result;
}

inline ThreadPool::Worker::Worker(ThreadPool* pool)
  : _pool(pool)
{
}

inline void ThreadPool::Worker::operator()(std::size_t nb_task)
{
  for (std::size_t i = 0; i != nb_task || nb_task == 0; i++)
  {
    _pool->_waiting_threads += 1;
    std::unique_lock<std::mutex> lock(_pool->_tasks_lock);
    _pool->_cv_variable.wait(
      lock, [&] { return _pool->_stop || !_pool->_tasks.empty(); });
    _pool->_waiting_threads -= 1;
    _pool->_working_threads += 1;

    if (_pool->_stop)
      return;

    // Fetch task
    std::packaged_task<void()> task = std::move(_pool->_tasks.front());
    _pool->_tasks.pop();

    // Release lock and exec task
    lock.unlock();
    task();
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
