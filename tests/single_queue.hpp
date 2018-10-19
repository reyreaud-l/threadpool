#pragma once

#include "threadpool.hpp"
#include <chrono>
#include <future>
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <utility>

class SingleQueue : public ::testing::Test
{
protected:
  virtual void SetUp() final
  {
    single_thread_pool = std::unique_ptr<ThreadPool::ThreadPool<>>(
      new ThreadPool::ThreadPool<>(1));
    variable_thread_pool = std::unique_ptr<ThreadPool::ThreadPool<>>(
      new ThreadPool::ThreadPool<>(2, 3));
    multiple_thread_pool = std::unique_ptr<ThreadPool::ThreadPool<>>(
      new ThreadPool::ThreadPool<>(2));
  }
  std::unique_ptr<ThreadPool::ThreadPool<>> single_thread_pool;
  std::unique_ptr<ThreadPool::ThreadPool<>> variable_thread_pool;
  std::unique_ptr<ThreadPool::ThreadPool<>> multiple_thread_pool;
};
