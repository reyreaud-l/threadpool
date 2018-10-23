#pragma once

#include "threadpool.hpp"
#include <chrono>
#include <future>
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <utility>

struct DummyHooks : public ThreadPool::Hooks
{
  void pre_task_hook() final
  {
    check_pre_task = true;
  }

  std::atomic<bool> check_pre_task;

  void post_task_hook() final
  {
    check_post_task = true;
  }

  std::atomic<bool> check_post_task;

  void on_worker_add() final
  {
    check_worker_add++;
  }

  std::atomic<int> check_worker_add;

  void on_worker_die() final
  {
    check_worker_die++;
  }

  std::atomic<int> check_worker_die;
};

class TestHooks : public ::testing::Test
{
protected:
  virtual void SetUp() final
  {
    pool = std::unique_ptr<ThreadPool::ThreadPool<>>(
      new ThreadPool::ThreadPool<>(2));
    hooks = std::shared_ptr<DummyHooks>(new DummyHooks());
  }
  std::unique_ptr<ThreadPool::ThreadPool<>> pool;
  std::shared_ptr<DummyHooks> hooks;
};
