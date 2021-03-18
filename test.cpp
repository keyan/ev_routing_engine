#include <ctime>
#include <fstream>
#include <string.h>

#include "network.h"
#include "router.h"

#define CLOCKS_PER_MS (CLOCKS_PER_SEC / 1000)

int main(int argc, char **argv) {
  int run_count = 200;
  Router routing_engine = Router(network);

  std::ofstream file;
  file.open("run_checker.sh");
  file << "#!/bin/bash\n";

  double total_query_times = 0.0;
  for (int i = 0; i < run_count; ++i) {
    int source = std::rand() % network.size();
    int target = std::rand() % network.size();
    if (source == target) {
      continue;
    }

    std::string source_name = network.at(source).name;
    std::string target_name = network.at(target).name;

    std::clock_t begin = std::clock();
    std::string result = routing_engine.route(source_name, target_name);
    std::clock_t end = std::clock();
    total_query_times += double(end - begin) / CLOCKS_PER_MS;

    file << "./reference_linux "
         << "\"" << result << "\"\n";
  }

  if (argc == 2 && strcmp(argv[1], "-r") == 0) {
    file << "echo '# "
         << "Total search time: " << std::to_string(total_query_times)
         << "ms - Average search time: " << std::to_string(total_query_times / double(run_count))
         << "ms"
         << "'\n";
  }
  file.close();

  return 0;
}
