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
  ThreadPool(std::size_t pool_size)
  {
    for (std::size_t i = 0; i < pool_size; i++)
    {
      _pool.emplace_back([&] {
        for (;;)
        {
          // Wait for a task to come in.
          std::unique_lock<std::mutex> lock(this->_tasks_lock);
          this->_cv_variable.wait(
            lock, [&] { return this->_stop || !this->_tasks.empty(); });

          if (this->_stop)
            return;

          // Fetch task
          std::packaged_task<void()> task = std::move(this->_tasks.front());
          this->_tasks.pop();

          // Release lock and exec task
          lock.unlock();
          task();
        }
      });
    }
  }

  ~ThreadPool()
  {
    this->stop();
    this->clean();
  }

  template <typename Function, typename... Args>
  auto run(Function&& f, Args&&... args)
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

  // Stop the thread pool and notify all workers to stop
  void stop()
  {
    std::lock_guard<std::mutex> lock(this->_tasks_lock);
    this->_stop = true;
    this->_cv_variable.notify_all();
  }

private:
  void clean()
  {
    for (auto& t : _pool)
      t.join();
  }
  std::vector<std::thread> _pool;
  std::queue<std::packaged_task<void()>> _tasks;
  std::condition_variable _cv_variable;
  bool _stop = false;
  std::mutex _tasks_lock;
};
