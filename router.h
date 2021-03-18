#pragma once
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>

#include "label.h"
#include "network.h"
#include "utils.h"

using NodeToLabelMap = std::unordered_map<NodeID, Label>;

class Router {
public:
  // Constructor builds an adjencey list representing the complete graph minus impossible to reach
  // nodes.
  Router(const std::vector<Station> &network) : network_(network) {
    graph_ = std::vector<std::vector<std::pair<NodeID, Kilometers>>>(
        network.size(), std::vector<std::pair<NodeID, Kilometers>>());

    // Created directed graph limited by battery radius.
    for (NodeID i = 0; i < network.size(); ++i) {
      node_name_map_[network_.at(i).name] = i;

      for (NodeID j = i + 1; j < network.size(); ++j) {
        if (i == j) {
          exit(1);
        }
        Kilometers travel_dist = calculate_travel_km(i, j);
        if (calculate_travel_km(i, j) > MAX_CHARGE) {
          continue;
        }

        graph_.at(i).emplace_back(std::make_pair(j, travel_dist));
        graph_.at(j).emplace_back(std::make_pair(i, travel_dist));
      }
    }
  }

  // Runs a modified version of Dijkstra's similar to bicriteria Dijkstra's and returns
  // a string result showing the route from the source and target provided.
  std::string route(std::string source_name, std::string target_name);

private:
  const std::vector<Station> &network_;
  // Maps a node's geographical string name to a NodeID.
  std::unordered_map<std::string, NodeID> node_name_map_;

  // Adjacency list representation of network. The network is a complete graph in theory, but
  // some edges can be pruned because not all connections are possible on a full charge.
  std::vector<std::vector<std::pair<NodeID, Kilometers>>> graph_;

  // Traverses the shortest path tree built by routing to create the result output.
  std::string build_result_string(const NodeToLabelMap &shortest_path_tree, NodeID source,
                                  NodeID target);

  Kilometers calculate_travel_km(NodeID, NodeID);
};
