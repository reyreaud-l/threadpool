#pragma once

#include <future>
#include <gtest/gtest.h>
#include <memory>
#include <thread>

#include "threadpool.hpp"

class ThreadPoolTest : public ::testing::Test
{
protected:
  virtual void SetUp() final
  {
    single_thread_pool =
      std::unique_ptr<ThreadPool::ThreadPool>(new ThreadPool::ThreadPool(1));
    multiple_thread_pool =
      std::unique_ptr<ThreadPool::ThreadPool>(new ThreadPool::ThreadPool(2));
  }
  std::unique_ptr<ThreadPool::ThreadPool> single_thread_pool;
  std::unique_ptr<ThreadPool::ThreadPool> multiple_thread_pool;
};
