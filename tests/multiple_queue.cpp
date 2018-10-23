#include "multiple_queue.hpp"

TEST_F(MultipleQueue, SingleThreadStartStop)
{
  ASSERT_FALSE(single_thread_pool->is_stopped());
  single_thread_pool->stop();
  ASSERT_TRUE(single_thread_pool->is_stopped());
}

TEST_F(MultipleQueue, SingleThreadSingleTask)
{
  std::future<bool> result;

  result = single_thread_pool->run([]() -> bool { return true; });

  ASSERT_TRUE(result.get());
}

TEST_F(MultipleQueue, SingleThreadMultipleTask)
{
  std::size_t nb_tests = 10;
  std::vector<std::future<bool>> results;

  for (std::size_t i = 0; i < nb_tests; i++)
    results.push_back(single_thread_pool->run([]() -> bool { return true; }));

  for (std::size_t i = 0; i < nb_tests; i++)
    ASSERT_TRUE(results[i].get());
}

TEST_F(MultipleQueue, MultipleThreadStartStop)
{
  ASSERT_FALSE(multiple_thread_pool->is_stopped());
  multiple_thread_pool->stop();
  ASSERT_TRUE(multiple_thread_pool->is_stopped());
}

TEST_F(MultipleQueue, MultipleThreadSingleTask)
{
  std::future<bool> result;

  result = multiple_thread_pool->run([]() -> bool { return true; });

  ASSERT_TRUE(result.get());
}

TEST_F(MultipleQueue, MultipleThreadMultipleTask)
{
  std::size_t nb_tests = 2;
  std::vector<std::future<bool>> results;

  for (std::size_t i = 0; i < nb_tests; i++)
    results.push_back(multiple_thread_pool->run([]() -> bool { return true; }));

  for (std::size_t i = 0; i < nb_tests; i++)
    ASSERT_TRUE(results[i].get());
}
