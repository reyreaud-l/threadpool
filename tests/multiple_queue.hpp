#pragma once

#include "threadpool.hpp"
#include <chrono>
#include <future>
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <utility>

class MultipleQueue : public ::testing::Test
{
protected:
  virtual void SetUp() final
  {
    single_thread_pool =
      std::unique_ptr<ThreadPool::ThreadPool<ThreadPool::MultipleQueue>>(
        new ThreadPool::ThreadPool<ThreadPool::MultipleQueue>(1));
    multiple_thread_pool =
      std::unique_ptr<ThreadPool::ThreadPool<ThreadPool::MultipleQueue>>(
        new ThreadPool::ThreadPool<ThreadPool::MultipleQueue>(2));
  }
  std::unique_ptr<ThreadPool::ThreadPool<ThreadPool::MultipleQueue>>
    single_thread_pool;
  std::unique_ptr<ThreadPool::ThreadPool<ThreadPool::MultipleQueue>>
    multiple_thread_pool;
};
