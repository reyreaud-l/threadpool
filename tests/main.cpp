#include "threadpool.hpp"
#include <future>

int main()
{
  ThreadPool a(1);

  std::size_t nb_tests = 10;
  std::vector<std::future<void>> results;
  results.reserve(nb_tests);

  for (std::size_t i = 0; i < nb_tests; i++)
  {
    std::cout << "Launching " << i << std::endl;
    results[i] =
      a.run([i]() -> void { std::cout << "running: " << i << std::endl; });
  }

  for (std::size_t i = 0; i < nb_tests; i++)
  {
    std::cout << "Waiting " << i << std::endl;
    results[i].get();
  }

  a.stop();
  return 0;
}
