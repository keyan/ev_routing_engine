#include "network.h"
#include "router.h"

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cout << "Error: requires initial and final supercharger names" << std::endl;
    return -1;
  }

  std::string initial_charger_name = argv[1];
  std::string goal_charger_name = argv[2];

  if (initial_charger_name == goal_charger_name) {
    std::cout << "Error: initial and final superchargers cannot be identical" << std::endl;
    return -1;
  }

  Router routing_engine = Router(network);
  std::string result = routing_engine.route(initial_charger_name, goal_charger_name);
  std::cout << result << std::endl;

  return 0;
}
