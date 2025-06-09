#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>
#include <map>
#include <functional>
#include "polygon.hpp"
#include "commands.hpp"

int main(int argc, char *argv[])
{
  using namespace nehvedovich;
  using namespace std::placeholders;

  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " <input_file>\n";
    return 1;
  }

  std::vector< Polygon > polygons;

  {
    std::ifstream input(argv[1]);
    if (!input)
    {
      std::cerr << "Error: cannot open file " << argv[1] << '\n';
      return 1;
    }

    using input_it_t = std::istream_iterator< Polygon >;
    while (!input.eof())
    {
      std::copy(input_it_t(input), input_it_t(), std::back_inserter(polygons));
      if (input.fail())
      {
        input.clear();
        input.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
      }
    }
  }

  std::map< std::string, std::function< void(std::istream &, std::ostream &) > > commands {
      {"AREA", std::bind(areaCommand, _1, _2, std::cref(polygons))},
      {"MAX", std::bind(maxCommand, _1, _2, std::cref(polygons))},
      {"MIN", std::bind(minCommand, _1, _2, std::cref(polygons))},
      {"COUNT", std::bind(countCommand, _1, _2, std::cref(polygons))},
      {"LESSAREA", std::bind(lessAreaCommand, _1, _2, std::cref(polygons))},
      {"INFRAME", std::bind(inFrameCommand, _1, _2, std::cref(polygons))}};

  while (!std::cin.eof())
  {
    std::string cmd;
    std::cin >> cmd;
    if (cmd.empty())
    {
      continue;
    }

    try
    {
      auto it = commands.find(cmd);
      if (it != commands.end())
      {
        it->second(std::cin, std::cout);
      }
      else
      {
        throw std::runtime_error("Unknown command");
      }
    }
    catch (...)
    {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
      std::cout << "<INVALID COMMAND>\n";
    }
  }

  return 0;
}
