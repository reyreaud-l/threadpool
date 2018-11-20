#include <chrono>
#include <set>
#include <vector>

#include "benchmark/benchmark.h"
#include "threadpool.hpp"
#include "utils.hpp"

static void bm_work_tasks(benchmark::State& state)
{
  while (state.KeepRunning())
  {
    ThreadPool::ThreadPool pool;
    std::vector<std::future<std::size_t>> results;
    std::set<std::thread::id> threads_id;

    auto i_end = state.range(0);
    for (int i = 0; i < i_end; i++)
      results.push_back(pool.run([&state, i]() {
        std::string value = bench::expensive(static_cast<std::size_t>(state.range(0)));
        benchmark::DoNotOptimize(value.data());
        benchmark::ClobberMemory();
        return value.size();
      }));
    for (int i = 0; i < i_end; i++)
      results[i].get();
  }
}

BENCHMARK(bm_work_tasks)->Range(8, 8 << 10)->Unit(benchmark::kMillisecond);

static void bm_blocking_tasks(benchmark::State& state)
{
  while (state.KeepRunning())
  {
    ThreadPool::ThreadPool pool;
    std::vector<std::future<void>> results;
    std::set<std::thread::id> threads_id;

    auto i_end = state.range(0);
    for (int i = 0; i < i_end; i++)
      results.push_back(
        pool.run([i]() { std::this_thread::sleep_for(std::chrono::microseconds(10)); }));
    for (int i = 0; i < i_end; i++)
      results[i].get();
  }
}

BENCHMARK(bm_blocking_tasks)->Range(8, 8 << 15)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
