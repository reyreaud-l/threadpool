#include "multiple_queue.hpp"

namespace ThreadPool
{
// MultipleQueue implementation
// public:

MultipleQueue::MultipleQueue(std::size_t pool_size, std::size_t max_pool_size)
  : ThreadPoolBase(pool_size, max_pool_size)
{
  start_pool();
}

MultipleQueue::~MultipleQueue()
{
  stop();
  clean();
}

void MultipleQueue::stop()
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

// MultipleQueue implementation
// private:

void MultipleQueue::start_pool()
{
  for (std::size_t i = 0; i < pool_size; i++)
  {
    add_worker();
  }
}

void MultipleQueue::clean()
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

void MultipleQueue::add_worker(std::size_t nb_task)
{
  auto w = std::unique_ptr<Worker>(new Worker(this));
  pool.emplace_back(std::ref(*w), nb_task);
  workers.push_back(std::move(w));
}

void MultipleQueue::check_spawn_single_worker()
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

// Worker implementation
// public:

MultipleQueue::Worker::Worker(MultipleQueue* pool)
  : pool(pool)
{
}

void MultipleQueue::Worker::operator()(std::size_t nb_task)
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

    // Release lock and exec task
    lock.unlock();
    task();

    // Task done
    CALL_HOOK_WORKER(post_task_hook);
    pool->working_threads -= 1;
  }
}
} // namespace ThreadPool
