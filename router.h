#pragma once
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "network.h"
#include "utils.h"

using NodeID = uint16_t;
using Weight = Milliseconds;

struct Label {
  Label(NodeID node_id, int label_id, Weight total_weight, Weight charge_time,
        Kilometers state_of_charge, NodeID parent)
      : node_id(node_id), label_id(label_id), total_weight(total_weight), charge_time(charge_time),
        state_of_charge(state_of_charge), parent(parent) {}

  // Non-unique between Labels, references the index into network_ where lat/lng/name info is
  // stored.
  NodeID node_id;
  // A unique identifier (node_id is not unique) so deletes from pq can be tracked.
  int label_id;
  // Total travel time from the search source to this node.
  Weight total_weight;
  // The amount of time spent charging at the parent node.
  Weight charge_time;
  // The amount of charge remaining when arriving at this node.
  Kilometers state_of_charge;
  // The node visited immediately prior.
  NodeID parent;

  bool operator<(const Label &other) const { return total_weight < other.total_weight; }

  bool operator>(const Label &other) const { return total_weight > other.total_weight; }

  // Returns true if this Label is better on both time and charge criteria over another.
  bool dominates(const Label &other) const {
    return total_weight < other.total_weight && state_of_charge > other.state_of_charge;
  }

  // Debugging helper to show contents.
  std::string to_string() {
    return ("label_id: " + std::to_string(label_id) + ", node_id: " + std::to_string(node_id) +
            ", charge_time: " + std::to_string(charge_time) + ", SoC: " +
            std::to_string(state_of_charge) + ", weight: " + std::to_string(total_weight));
  }
};

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
