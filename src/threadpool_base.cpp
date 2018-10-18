#include "threadpool_base.hpp"

namespace ThreadPool
{
bool ThreadPoolBase::is_stopped() const
{
  return stopped;
}

std::size_t ThreadPoolBase::threads_available() const
{
  return waiting_threads.load();
}

std::size_t ThreadPoolBase::threads_working() const
{
  return working_threads.load();
}

void ThreadPoolBase::register_hooks(std::shared_ptr<Hooks> hooks)
{
  this->hooks = hooks;
}

} // namespace ThreadPool
