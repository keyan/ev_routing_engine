#include <fstream>

#include "network.h"
#include "router.h"

int main(int argc, char **argv) {
  Router routing_engine = Router(network);

  std::ofstream file;
  file.open("run_checker.sh");
  file << "#!/bin/bash\n";

  for (int i = 0; i < 200; ++i) {
    int source = std::rand() % network.size();
    int target = std::rand() % network.size();
    if (source == target) {
      continue;
    }

    std::string source_name = network.at(source).name;
    std::string target_name = network.at(target).name;
    std::string result = routing_engine.route(source_name, target_name);

    file << "./checker_osx "
         << "\"" << result << "\"\n";
  }
  file.close();

  return 0;
}
