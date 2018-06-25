#include "tests.hpp"

class MultipleThread : public ::testing::Test
{
protected:
  virtual void SetUp() final
  {
    pool = std::unique_ptr<ThreadPool>(new ThreadPool(10));
  }
  std::unique_ptr<ThreadPool> pool;
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
  std::size_t nb_tests = 10;
  std::vector<std::future<bool>> results;
  results.reserve(nb_tests);

  for (std::size_t i = 0; i < nb_tests; i++)
    results[i] = pool->run([]() -> bool { return true; });

  for (std::size_t i = 0; i < nb_tests; i++)
    ASSERT_TRUE(results[i].get());
}

TEST_F(MultipleThread, OccupyAllThreads)
{
  std::size_t nb_tests = 10;
  std::vector<std::future<bool>> results;
  results.reserve(nb_tests);

  for (std::size_t i = 0; i < nb_tests; i++)
    results[i] = pool->run([]() -> bool {
      // Occupy thread for 3 secs
      std::this_thread::sleep_for(std::chrono::seconds(3));
      return true;
    });
  // Wait for all tasks to be dispatched
  std::this_thread::sleep_for(std::chrono::seconds(1));

  ASSERT_EQ(pool->threads_available(), 0);
  ASSERT_EQ(pool->threads_working(), 10);

  for (std::size_t i = 0; i < nb_tests; i++)
    ASSERT_TRUE(results[i].get());
}
