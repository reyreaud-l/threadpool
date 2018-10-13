#include "single_queue.hpp"

namespace ThreadPool
{
// SingleQueue implementation
// public:

SingleQueue::SingleQueue()
  : SingleQueue(std::thread::hardware_concurrency(),
                std::thread::hardware_concurrency())
{
}

SingleQueue::SingleQueue(std::size_t pool_size)
  : SingleQueue(pool_size, pool_size)

{
}

SingleQueue::SingleQueue(std::size_t pool_size, std::size_t max_pool_size)
  : ThreadPoolBase(pool_size, max_pool_size)
{
  this->start_pool();
}

SingleQueue::~SingleQueue()
{
  this->stop();
  this->clean();
}

void SingleQueue::stop()
{
  // Should stop also call clean and stop the threads ?
  std::lock_guard<std::mutex> lock(this->_tasks_lock);
  this->_stop = true;
  this->_cv_variable.notify_all();
}

// SingleQueue implementation
// private:

void SingleQueue::start_pool()
{
  for (std::size_t i = 0; i < this->_pool_size; i++)
    this->add_worker();
}

void SingleQueue::clean()
{
  for (auto& t : _pool)
    if (t.joinable())
    {
      CALL_HOOK_POOL(on_worker_die);
      t.join();
    }
}

void SingleQueue::add_worker(std::size_t nb_task)
{
  // Instantiate a worker and emplace it in the pool.
  Worker w(this);
  _pool.emplace_back(w, nb_task);
}

void SingleQueue::check_spawn_single_worker()
{
  // Check if we are allowed to spawn a worker
  if (this->_max_pool_size > this->_pool_size)
    // Check if we have space to spawn a worker, and if it is valuable.
    if (this->_working_threads.load() + this->_waiting_threads.load() <
        this->_max_pool_size)
    {
      CALL_HOOK_POOL(on_worker_add);
      this->add_worker(1);
    }
}

// Worker implementation
// public:

SingleQueue::Worker::Worker(SingleQueue* pool)
  : _pool(pool)
{
}

void SingleQueue::Worker::operator()(std::size_t nb_task)
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

    CALL_HOOK_WORKER(pre_task_hook);

    _pool->_waiting_threads -= 1;
    _pool->_working_threads += 1;

    // Fetch task
    std::packaged_task<void()> task = std::move(_pool->_tasks.front());
    _pool->_tasks.pop();

    // Release lock and exec task
    lock.unlock();
    task();

    CALL_HOOK_WORKER(post_task_hook);
    // Task done
    _pool->_working_threads -= 1;
  }
}
} // namespace ThreadPool
