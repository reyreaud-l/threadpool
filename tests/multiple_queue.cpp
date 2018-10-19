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

TEST_F(MultipleQueue, MultipleThreadOccupyAllThreads)
{
  // Check that all threads are working under heavy load
  std::size_t nb_tests = 2;
  std::vector<std::future<std::pair<std::thread::id, bool>>> results;
  std::set<std::thread::id> threads_id;

  for (std::size_t i = 0; i < nb_tests; i++)
    results.push_back(multiple_thread_pool->run([]() {
      // Occupy thread for 5 secs
      std::this_thread::sleep_for(std::chrono::seconds(5));
      return std::make_pair(std::this_thread::get_id(), true);
    }));
  // Wait for all tasks to be dispatched
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Check that all threads are working
  ASSERT_EQ(multiple_thread_pool->threads_available(), 0);
  ASSERT_EQ(multiple_thread_pool->threads_working(), 2);

  // Check results
  for (std::size_t i = 0; i < nb_tests; i++)
  {
    auto p = results[i].get();
    ASSERT_TRUE(p.second);

    // As all threads should be occupied, no task should run in the same thread
    ASSERT_TRUE(threads_id.insert(p.first).second);
  }
}
