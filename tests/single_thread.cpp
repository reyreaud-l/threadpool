#include "tests.hpp"

class SingleThread : public ::testing::Test
{
protected:
  virtual void SetUp() final
  {
    pool = std::unique_ptr<ThreadPool<SQMW>>(new ThreadPool<SQMW>(1));
  }
  std::unique_ptr<ThreadPool<SQMW>> pool;
};

TEST_F(SingleThread, StartStop)
{
  ASSERT_FALSE(pool->is_stop());
  pool->stop();
  ASSERT_TRUE(pool->is_stop());
}

TEST_F(SingleThread, SingleTask)
{
  std::future<bool> result;

  result = pool->run([]() -> bool { return true; });

  ASSERT_TRUE(result.get());
}

TEST_F(SingleThread, MultipleTask)
{
  std::size_t nb_tests = 10;
  std::vector<std::future<bool>> results;

  for (std::size_t i = 0; i < nb_tests; i++)
    results.push_back(pool->run([]() -> bool { return true; }));

  for (std::size_t i = 0; i < nb_tests; i++)
    ASSERT_TRUE(results[i].get());
}
