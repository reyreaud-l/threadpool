#include <map>

namespace bench
{
// Expensive allocation of std::map, querying, and string comparison,
// therefore threads are busy. This example is primarily used for benchmarking.
std::string expensive(std::size_t work_to_do)
{
  std::map<std::size_t, std::string> container;

  for (std::size_t i = 0; i < work_to_do; ++i)
  {
    container.emplace(i, std::to_string(i));
  }

  for (std::size_t i = 0; i < work_to_do; ++i)
  {
    std::string const& item = container[i];
    if (item != std::to_string(i))
    {
      throw std::runtime_error("Uh oh, this should never happen");
    }
  }

  std::string result = "Expensive work is finished";

  return result;
}
} // namespace bench
