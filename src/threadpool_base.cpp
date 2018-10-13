#include "threadpool_base.hpp"

namespace ThreadPool
{
bool ThreadPoolBase::is_stop() const
{
  return this->_stop;
}

std::size_t ThreadPoolBase::threads_available() const
{
  return this->_waiting_threads.load();
}

std::size_t ThreadPoolBase::threads_working() const
{
  return this->_working_threads.load();
}

void ThreadPoolBase::register_hooks(std::shared_ptr<Hooks> hooks)
{
  _hooks = hooks;
}

} // namespace ThreadPool
