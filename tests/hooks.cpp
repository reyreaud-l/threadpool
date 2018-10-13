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

  void on_worker_add() final
  {
    check_worker_add++;
  }

  int check_worker_add = 0;

  void on_worker_die() final
  {
    check_worker_die++;
  }

  int check_worker_die = 0;
};

class THooks : public ::testing::Test
{
protected:
  virtual void SetUp() final
  {
    pool = std::unique_ptr<ThreadPool::ThreadPool<>>(
      new ThreadPool::ThreadPool<>(2));
    hooks = std::shared_ptr<TestHooks>(new TestHooks());
  }
  std::unique_ptr<ThreadPool::ThreadPool<>> pool;
  std::shared_ptr<TestHooks> hooks;
};

TEST_F(THooks, RegisterTHooksNoCall)
{
  pool->register_hooks(hooks);
  ASSERT_FALSE(hooks->check_pre_task);
  ASSERT_FALSE(hooks->check_post_task);
}

TEST_F(THooks, CheckTaskTHooksCalled)
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

// This tests abuse from sleep to simulate heaavy coreload.
// In practice if a task is really quick to run and you launch them in a quick
// succession, a new thread might not spawn.
TEST_F(THooks, TestWorkerTHooksCalled)
{
  pool = std::unique_ptr<ThreadPool::ThreadPool<>>(
    new ThreadPool::ThreadPool<>(1, 3));
  pool->register_hooks(hooks);

  std::size_t nb_tests = 3;
  std::vector<std::future<int>> results;
  std::set<std::thread::id> threads_id;

  for (std::size_t i = 0; i < nb_tests; i++)
  {
    results.push_back(pool->run([]() {
      // Occupy thread for 5 secs
      std::this_thread::sleep_for(std::chrono::seconds(5));
      return 0;
    }));
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  // Check results
  for (std::size_t i = 0; i < nb_tests; i++)
    results[i].get();

  // Delete the threadpool
  pool.reset();

  // 2 worker should have been added
  ASSERT_EQ(hooks->check_worker_add, 2);
  ASSERT_EQ(hooks->check_worker_die, 3);
}
