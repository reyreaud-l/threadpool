#include "tests.hpp"

class MultipleThread : public ::testing::Test
{
protected:
  virtual void SetUp() final
  {
    pool = std::unique_ptr<ThreadPool::ThreadPool<>>(
      new ThreadPool::ThreadPool<>(2));
  }
  std::unique_ptr<ThreadPool::ThreadPool<>> pool;
};

TEST_F(MultipleThread, StartStop)
{
  ASSERT_FALSE(pool->is_stop());
  pool->stop();
  ASSERT_TRUE(pool->is_stop());
}

TEST_F(MultipleThread, SingleTask)
{
  std::future<bool> result;

  result = pool->run([]() -> bool { return true; });

  ASSERT_TRUE(result.get());
}

TEST_F(MultipleThread, MultipleTask)
{
  std::size_t nb_tests = 2;
  std::vector<std::future<bool>> results;

  for (std::size_t i = 0; i < nb_tests; i++)
    results.push_back(pool->run([]() -> bool { return true; }));

  for (std::size_t i = 0; i < nb_tests; i++)
    ASSERT_TRUE(results[i].get());
}

TEST_F(MultipleThread, OccupyAllThreads)
{
  // Check that all threads are working under heavy load
  std::size_t nb_tests = 2;
  std::vector<std::future<std::pair<std::thread::id, bool>>> results;
  std::set<std::thread::id> threads_id;

  for (std::size_t i = 0; i < nb_tests; i++)
    results.push_back(pool->run([]() {
      // Occupy thread for 5 secs
      std::this_thread::sleep_for(std::chrono::seconds(5));
      return std::make_pair(std::this_thread::get_id(), true);
    }));
  // Wait for all tasks to be dispatched
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Check that all threads are working
  ASSERT_EQ(pool->threads_available(), 0);
  ASSERT_EQ(pool->threads_working(), 2);

  // Check results
  for (std::size_t i = 0; i < nb_tests; i++)
  {
    auto p = results[i].get();
    ASSERT_TRUE(p.second);

    // As all threads should be occupied, no task should run in the same thread
    ASSERT_TRUE(threads_id.insert(p.first).second);
  }
}
