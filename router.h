#pragma once
#include <cstdint>
#include <limits>
#include <vector>
#include <unordered_map>

#include "utils.h"
#include "network.h"

using NodeID = uint16_t;
using Weight = Milliseconds;

constexpr NodeID MAX_NODE_ID = std::numeric_limits<NodeID>::max();
constexpr Weight MAX_WEIGHT = std::numeric_limits<Weight>::max();

struct WeightedNode {
  WeightedNode(NodeID node_id, int counter, Weight total_weight, Weight charge_time,
               Kilometers state_of_charge, NodeID parent = MAX_NODE_ID)
      : node_id(node_id)
      , counter(counter)
      , total_weight(total_weight)
      , charge_time(charge_time)
      , state_of_charge(state_of_charge)
      , parent(parent) {}

  NodeID node_id;
  // A unique identifier (node_id is not unique) so deletes from pq can be tracked.
  int counter;
  // Total travel time from the search source to this node.
  Weight total_weight;
  // The amount of time spent charging at the parent node.
  Weight charge_time;
  // The amount of charge remaining when arriving at this node.
  Kilometers state_of_charge;
  NodeID parent;

  bool operator<(const WeightedNode& other) const {
    return total_weight < other.total_weight;
  }

  bool operator>(const WeightedNode& other) const {
    return total_weight > other.total_weight;
  }

  // Returns true if this WeightedNode is better on both time and charge criteria.
  bool dominates(const WeightedNode& other) {
    return total_weight < other.total_weight && state_of_charge > other.state_of_charge;
  }

  // Debugging helper to show contents.
  std::string to_string(std::string name) {
    return "name: " + name + ", charge_time: " + std::to_string(charge_time) + ", SoC: " + std::to_string(state_of_charge);
  }
};

using NodeMap = std::unordered_map<NodeID, WeightedNode>;

// For each NodeID we keep a vector of "labels", which indicate multiple ways to
// arrive to the same NodeID. All labels in the vector are non-dominating in respect
// to total_weight and state_of_charge. That is, all labels are Pareto optimal.
using LabelMap = std::unordered_map<NodeID, std::vector<WeightedNode> >;

class Router {
  public:
    Router(const std::vector<row>& network, std::string source_name, std::string target_name)
      : network_(network) {
      source_node_id_ = network_.size() - 1;
      target_node_id_ = network_.size() - 1;

      graph_ = std::vector<std::vector<NodeID> >(network.size(), std::vector<NodeID>());

      // Created directed graph limited by battery radius.
      for (NodeID i = 0; i < network.size() - 1; ++i) {
        // Find NodeID for source/target without building a map.
        if (network_.at(i).name == source_name) { source_node_id_ = i; }
        if (network_.at(i).name == target_name) { target_node_id_ = i; }

        for (NodeID j = i + 1; j < network.size(); ++j) {
          if (calculate_travel_km(i, j) <= MAX_CHARGE) {
            graph_.at(i).push_back(j);
            graph_.at(j).push_back(i);
          }
        }
      }
    }

    // Return a string result showing the route from the source and target provided
    // to the Router() constructor.
    std::string route();

  private:
    const std::vector<row>& network_;
    NodeID source_node_id_;
    NodeID target_node_id_;

    // Adjacency list representation of network. The network is a complete graph in theory, but
    // some edges can be pruned because not all connections are possible on a full charge.
    std::vector<std::vector<NodeID> > graph_;

    // Traverses the shortest path tree built by routing to create the result output.
    std::string build_result_string(const NodeMap& shortest_path_tree);

    Kilometers calculate_travel_km(NodeID, NodeID);
    // Weight calculate_travel_secs(NodeID, NodeID);
};
