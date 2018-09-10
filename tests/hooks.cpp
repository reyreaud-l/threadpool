#include "tests.hpp"

struct TestHooks : public ThreadPool::Hooks
{
  void pre_task_hook() final
  {
    check_pre_task = true;
  }

  bool check_pre_task = false;

  void post_task_hook() final
  {
    check_post_task = true;
  }

  bool check_post_task = false;
};

class Hooks : public ::testing::Test
{
protected:
  virtual void SetUp() final
  {
    pool = std::unique_ptr<ThreadPool>(new ThreadPool(1));
    hooks = std::shared_ptr<TestHooks>(new TestHooks());
  }
  std::unique_ptr<ThreadPool> pool;
  std::shared_ptr<TestHooks> hooks;
};

TEST_F(Hooks, RegisterHooksNoCall)
{
  pool->register_hooks(hooks);
  ASSERT_FALSE(hooks->check_pre_task);
  ASSERT_FALSE(hooks->check_post_task);
}

TEST_F(Hooks, CheckAllHooksCalled)
{
  pool->register_hooks(hooks);
  ASSERT_FALSE(hooks->check_pre_task);
  ASSERT_FALSE(hooks->check_post_task);

  auto res = pool->run([]() { return 0; });
  res.wait();
  // Sometimes the thread running the test make the assert before the post_task
  // hook is called by the worker. We wait a bit of time to make sure that the
  // worker has time to call the hook before we assert.
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  ASSERT_TRUE(hooks->check_pre_task);
  ASSERT_TRUE(hooks->check_post_task);
}

TEST_F(Hooks, TestCopyHook)
{
  // Test the overloading which makes a copy
  {
    TestHooks test;
    pool->register_hooks(test);
    auto res = pool->run([]() { return 0; });
    res.wait();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // copy was made, so it should be false
    ASSERT_FALSE(hooks->check_pre_task);
    ASSERT_FALSE(hooks->check_post_task);
  }
  // Check that a copy is made. If no copy were made, this would crash
  auto res = pool->run([]() { return 0; });
  res.wait();
}
