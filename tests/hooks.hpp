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

  bool check_pre_task = false;

  void post_task_hook() final
  {
    check_post_task = true;
  }

  bool check_post_task = false;

  void on_worker_add() final
  {
    check_worker_add++;
  }

  int check_worker_add = 0;

  void on_worker_die() final
  {
    check_worker_die++;
  }

  int check_worker_die = 0;
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
