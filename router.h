#pragma once
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "label.h"
#include "network.h"
#include "utils.h"

using NodeToLabelMap = std::unordered_map<NodeID, Label>;

class Router {
public:
  // Constructor builds an adjencey list representing the complete graph minus impossible to reach
  // nodes.
  Router(const std::array<row, 303> &network) : network_(network) {
    graph_ = std::vector<std::vector<NodeID>>(network.size(), std::vector<NodeID>());

    // Created directed graph limited by battery radius.
    for (NodeID i = 0; i < network.size(); ++i) {
      node_name_map_[network_.at(i).name] = i;

      for (NodeID j = i + 1; j < network.size(); ++j) {
        if (i == j) {
          exit(1);
        }
        if (calculate_travel_km(i, j) <= MAX_CHARGE) {
          graph_.at(i).push_back(j);
          graph_.at(j).push_back(i);
        }
      }
    }
  }

  // Runs a modified version of Dijkstra's similar to bicriteria Dijkstra's and returns
  // a string result showing the route from the source and target provided.
  std::string route(std::string source_name, std::string target_name);

private:
  const std::array<row, 303> &network_;
  // Maps a node's geographical string name to a NodeID.
  std::unordered_map<std::string, NodeID> node_name_map_;

  // Adjacency list representation of network. The network is a complete graph in theory, but
  // some edges can be pruned because not all connections are possible on a full charge.
  std::vector<std::vector<NodeID>> graph_;

  // Traverses the shortest path tree built by routing to create the result output.
  std::string build_result_string(const NodeToLabelMap &shortest_path_tree, NodeID source,
                                  NodeID target);

  Kilometers calculate_travel_km(NodeID, NodeID);
};
