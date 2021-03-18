#pragma once
#include <string>

#include "utils.h"

// A Label indicates one possible way to arrive to a NodeID in the graph. A single NodeID
// can have many labels associated with it, indicating different parent nodes, total travel
// time to reach the node, amount of charging time, and state of the battery upon arrival.
struct Label {
  Label(NodeID node_id, int label_id, Weight total_weight, Weight charge_time,
        Kilometers state_of_charge, NodeID parent)
      : node_id(node_id), label_id(label_id), total_weight(total_weight), charge_time(charge_time),
        state_of_charge(state_of_charge), parent(parent) {}

  // Non-unique between Labels, references the index into network_ where lat/lng/name info is
  // stored.
  NodeID node_id;
  // A unique identifier for this label (node_id is not unique).
  int label_id;
  // Total travel time from the search source to this node.
  Weight total_weight;
  // The amount of time spent charging at the _parent_ node.
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
