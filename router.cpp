#include <functional>
#include <queue>
#include <utility>
#include <unordered_set>
// #include <cassert>

#include "router.h"

using MinHeapPriorityQueue = std::priority_queue<
    WeightedNode,
    std::vector<WeightedNode>,
    std::greater<std::vector<WeightedNode>::value_type> >;

std::string Router::route() {
  NodeMap shortest_path_tree;
  LabelMap label_map;
  std::unordered_set<int> deleted_labels;

  int counter = 0;
  MinHeapPriorityQueue node_queue;
  node_queue.emplace(WeightedNode(source_node_id_, counter++, 0, 0, MAX_CHARGE));

  while (!node_queue.empty()) {
    WeightedNode curr_node = node_queue.top();
    node_queue.pop();

    const NodeID& curr_node_id = curr_node.node_id;

    // std::priority_queue has no decrease-weight operation, instead do a "lazy
    // deletion" by keeping the old node in the pq and just ignoring it when it
    // is eventually popped.
    if (deleted_labels.count(curr_node.counter) == 1 || shortest_path_tree.count(curr_node_id) == 1) {
      continue;
    }
    shortest_path_tree.emplace(curr_node_id, curr_node);

    // Search is done.
    if (curr_node_id == target_node_id_) {
      break;
    }

    const row& curr_row = network_.at(curr_node_id);

    // Update weights for all neighbors not in the spt.
    // This is the main departure from standard dijkstra's. Instead of relaxing edges between
    // neighbors, we construct "labels" up to 3 per neighbor, and try to merge them into the LabelMap.
    // Any non-dominated labels are also added to the priority queue.
    for (const NodeID& adj_node_id : graph_.at(curr_node_id)) {
      if (shortest_path_tree.count(adj_node_id) == 1) {
        continue;
      }

      Kilometers dist_to_neighbor = calculate_travel_km(curr_node_id, adj_node_id);
      Weight direct_weight_to_neighbor = convert_km_to_ms_travel(dist_to_neighbor);

      // Three possible label cases.
      std::vector<WeightedNode> labels;
      // 1. Go to neighbor without any charging, if possible.
      if (dist_to_neighbor <= curr_node.state_of_charge) {
        labels.emplace_back(
            WeightedNode(
              adj_node_id, counter++, curr_node.total_weight + direct_weight_to_neighbor,
              // curr_node.charge_time, curr_node.state_of_charge - dist_to_neighbor, curr_node_id));
              0, curr_node.state_of_charge - dist_to_neighbor, curr_node_id));
      }
      // 2. Do a full recharge, if needed.
      if (curr_node.state_of_charge < MAX_CHARGE) {
        Weight addtl_charge_time = time_to_full_charge(curr_node.state_of_charge, curr_row.rate);
        labels.emplace_back(
            WeightedNode(
              adj_node_id, counter++, curr_node.total_weight + direct_weight_to_neighbor + addtl_charge_time,
              // curr_node.charge_time + addtl_charge_time, MAX_CHARGE - dist_to_neighbor, curr_node_id));
              addtl_charge_time, MAX_CHARGE - dist_to_neighbor, curr_node_id));
      }
      // 3. Only charge enough to get to neighbor.
      if (curr_node.state_of_charge < MAX_CHARGE) {
        Weight addtl_charge_time = time_to_partial_charge(
            curr_node.state_of_charge, dist_to_neighbor, curr_row.rate);
        labels.emplace_back(
            WeightedNode(
              adj_node_id, counter++, curr_node.total_weight + direct_weight_to_neighbor + addtl_charge_time,
              // curr_node.charge_time + addtl_charge_time, 0, curr_node_id));
              addtl_charge_time, 0, curr_node_id));
      }

      auto search = label_map.find(adj_node_id);
      if (search == label_map.end()) {
        // No labels exist to dominate these ones, so add them all.
        for (auto label : labels) {
          node_queue.push(label);
        }
        label_map[adj_node_id] = std::move(labels);
      } else {
        ;
        // TODO label domination should be required for optimal routing, without it we should still
        // get possible thought not optimal routes.
        // for (auto label : labels) {
        //   for (auto other_label : search->second) {

        //   }
        // }
      }
    }
  }

  return build_result_string(shortest_path_tree);
}

std::string Router::build_result_string(const NodeMap& shortest_path_tree) {
  std::vector<std::string> names;
  std::vector<double> charge_times;

  WeightedNode curr = shortest_path_tree.at(target_node_id_);
  while (curr.node_id != source_node_id_) {
    names.push_back(network_.at(curr.node_id).name);
    charge_times.push_back(ms_to_hours(curr.charge_time));
    curr = shortest_path_tree.at(curr.parent);
  }

  // The last charge time is how long we charged at the source, so 0.
  charge_times.pop_back();

  std::string result = network_.at(source_node_id_).name;
  auto charge_it = charge_times.rbegin();
  for (auto name_it = names.rbegin(); name_it != names.rend(); ++name_it) {
    if (charge_it == charge_times.rend()) {
      result += ", " + *name_it;
    } else {
      result += ", " + *name_it + ", " + std::to_string(*charge_it);
      ++charge_it;
    }
  }

  return result;
}

Kilometers Router::calculate_travel_km(NodeID source, NodeID dest) {
  const row& source_row = network_.at(source);
  const row& dest_row = network_.at(dest);
  return haversine_dist(source_row.lat, source_row.lon, dest_row.lat, dest_row.lon);
}
