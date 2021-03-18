#include <vector>

#include "network.h"
#include "router.h"

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Error: requires initial and final supercharger names" << std::endl;
        return -1;
    }

    std::string initial_charger_name = argv[1];
    std::string goal_charger_name = argv[2];

    // Using vector makes testing easier.
    std::vector<row> network_v(network.begin(), network.end());
    Router routing_engine = Router(network_v, initial_charger_name, goal_charger_name);
    std::string result = routing_engine.route();
    std::cout << result << std::endl;

    return 0;
}
