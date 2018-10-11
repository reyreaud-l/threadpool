#include "tests.hpp"

class VariableThread : public ::testing::Test
{
protected:
  virtual void SetUp() final
  {
    pool = std::unique_ptr<ThreadPool::ThreadPool<ThreadPool::SQMW>>(
      new ThreadPool::ThreadPool<ThreadPool::SQMW>(2, 3));
  }
  std::unique_ptr<ThreadPool::ThreadPool<ThreadPool::SQMW>> pool;
};

// This tests abuse from sleep to simulate heavy coreload.
// In practice if a task is really quick to run and you launch them in a quick
// succession, a new thread might not spawn.
// Most importantly, this tests check that the threadpool under heavy loads
// adapt within it's limit and add another worker.
TEST_F(VariableThread, CheckSpawnThread)
{
  // Check that all threads are working under heavy load
  std::size_t nb_tests = 3;
  std::vector<std::future<std::pair<std::thread::id, bool>>> results;
  std::set<std::thread::id> threads_id;

  for (std::size_t i = 0; i < nb_tests; i++)
  {
    results.push_back(pool->run([]() {
      // Occupy thread for 5 secs
      std::this_thread::sleep_for(std::chrono::seconds(5));
      return std::make_pair(std::this_thread::get_id(), true);
    }));
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  // Wait for all tasks to be dispatched
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Check that all threads are working
  ASSERT_EQ(pool->threads_available(), 0);
  ASSERT_EQ(pool->threads_working(), 3);

  // Check results
  for (std::size_t i = 0; i < nb_tests; i++)
  {
    auto p = results[i].get();
    ASSERT_TRUE(p.second);

    // As all threads should be occupied, no task should run in the same thread
    ASSERT_TRUE(threads_id.insert(p.first).second);
  }
}
