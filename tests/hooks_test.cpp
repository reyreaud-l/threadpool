#include "hooks_test.hpp"

TEST_F(TestHooks, RegisterTestHooksNoCall)
{
  pool->register_hooks(hooks);
  ASSERT_FALSE(hooks->check_pre_task);
  ASSERT_FALSE(hooks->check_post_task);
}

TEST_F(TestHooks, CheckTaskTestHooksCalled)
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

TEST_F(TestHooks, TestWorkerTestHooksCalled)
{
  pool = std::unique_ptr<ThreadPool::ThreadPool>(
    new ThreadPool::ThreadPool(1, hooks));

  std::size_t nb_tests = 3;
  std::vector<std::future<int>> results;
  std::set<std::thread::id> threads_id;

  for (std::size_t i = 0; i < nb_tests; i++)
  {
    results.push_back(pool->run([]() {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      return 0;
    }));
  }

  // Check results
  for (std::size_t i = 0; i < nb_tests; i++)
    results[i].get();

  // Delete the threadpool
  pool.reset();

  // 1 worker should have been added
  // 1 worker should have been died
  ASSERT_EQ(hooks->check_worker_add, 1);
  ASSERT_EQ(hooks->check_worker_die, 1);
}
