#include "threadpool.hpp"

#include <iostream>

int main()
{
  ThreadPool::ThreadPool mypool(1);
  auto task = mypool.run([]() { std::cout << "Hello there!" << std::endl; });
  std::cout << "General Kenobi!" << std::endl;
  task.wait();
}
