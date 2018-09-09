#include <iostream>
#include <memory>

#include "threadpool.hpp"

struct TestHooks : public ThreadPool::Hooks
{
  void pre_task_hook() final
  {
    std::cout << "pre_task_hook\n";
  }

  void post_task_hook() final
  {
    std::cout << "post_task_hook\n";
  }
};

int main(void)
{
  ThreadPool pool(1);
  std::shared_ptr<TestHooks> hooks(new TestHooks());
  pool.register_hooks(hooks);

  auto res = pool.run([]() { return 0; });
  res.wait();
}
