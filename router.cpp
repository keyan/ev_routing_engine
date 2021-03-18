#include <algorithm>
#include <functional>
#include <queue>
#include <unordered_set>
#include <utility>

#include "router.h"

using MinHeapPriorityQueue =
    std::priority_queue<Label, std::vector<Label>, std::greater<std::vector<Label>::value_type>>;

std::string Router::route(std::string source_name, std::string target_name) {
  // For each NodeID we keep a vector of "labels", which indicate multiple ways to
  // arrive to the same NodeID. All labels in the vector are non-dominating in respect to
  // total_weight and state_of_charge. That is, all labels for a node are Pareto optimal.
  std::unordered_map<NodeID, std::vector<Label>> label_map;
  // Once a NodeID is added to the spt, we know the best Label to use to get to it.
  NodeToLabelMap shortest_path_tree;

  int label_id = 0;
  MinHeapPriorityQueue label_queue;
  // Used to keep track of removed Labels in the pq, we can't issue deletes because
  // std::priority_queue doesn't provide pointers to allow arbitrary deletes of items.
  std::unordered_set<int> deleted_labels;

  NodeID source_node_id = node_name_map_[source_name];
  NodeID target_node_id = node_name_map_[target_name];

  label_queue.emplace(Label(source_node_id, label_id++, 0, 0, MAX_CHARGE, source_node_id));
  while (!label_queue.empty()) {
    Label curr_label = label_queue.top();
    label_queue.pop();

    const NodeID &curr_label_id = curr_label.node_id;

    // std::priority_queue has no decrease-weight operation, instead do a "lazy
    // deletion" by keeping the old node in the pq and just ignoring it when it
    // is eventually popped.
    if (deleted_labels.count(curr_label.label_id) == 1 ||
        shortest_path_tree.count(curr_label_id) == 1) {
      continue;
    }
    shortest_path_tree.emplace(curr_label_id, curr_label);

    // Search is done.
    if (curr_label_id == target_node_id) {
      break;
    }

    const row &curr_row = network_.at(curr_label_id);

    // Update weights for all neighbors not in the spt.
    // This is the main departure from standard dijkstra's. Instead of relaxing edges between
    // neighbors, we construct "labels" up to 3 per neighbor, and try to merge them into the
    // LabelMap. Any non-dominated labels are also added to the priority queue.
    for (const NodeID &adj_node_id : graph_.at(curr_label_id)) {
      if (shortest_path_tree.count(adj_node_id) == 1) {
        continue;
      }

      Kilometers dist_to_neighbor = calculate_travel_km(curr_label_id, adj_node_id);
      Weight direct_weight_to_neighbor = convert_km_to_ms_travel(dist_to_neighbor);

      // Three possible label cases.
      std::vector<Label> labels;
      // 1. Go to neighbor without any charging, if possible.
      if (dist_to_neighbor <= curr_label.state_of_charge) {
        labels.emplace_back(Label(adj_node_id, label_id++,
                                  curr_label.total_weight + direct_weight_to_neighbor, 0,
                                  curr_label.state_of_charge - dist_to_neighbor, curr_label_id));
      }
      // 2. Do a full recharge, if needed.
      if (curr_label.state_of_charge < MAX_CHARGE) {
        Weight addtl_charge_time = time_to_full_charge(curr_label.state_of_charge, curr_row.rate);
        labels.emplace_back(
            Label(adj_node_id, label_id++,
                  curr_label.total_weight + direct_weight_to_neighbor + addtl_charge_time,
                  addtl_charge_time, MAX_CHARGE - dist_to_neighbor, curr_label_id));
      }
      // 3. Only charge enough to get to neighbor.
      if (curr_label.state_of_charge < MAX_CHARGE &&
          curr_label.state_of_charge < dist_to_neighbor) {
        Weight addtl_charge_time =
            time_to_partial_charge(curr_label.state_of_charge, dist_to_neighbor, curr_row.rate);
        labels.emplace_back(
            Label(adj_node_id, label_id++,
                  curr_label.total_weight + direct_weight_to_neighbor + addtl_charge_time,
                  addtl_charge_time, 0, curr_label_id));
      }

      // Update this nodes label bag. This is similar to "relaxing" edges in standard Dijkstra's.
      auto search = label_map.find(adj_node_id);
      if (search == label_map.end()) {
        // No labels exist to dominate these ones, so add them all.
        for (auto &label : labels) {
          label_queue.push(label);
        }
        label_map[adj_node_id] = std::move(labels);
      } else {
        std::vector<Label> &bag = search->second;

        for (auto &label : labels) {
          auto search = std::find_if(bag.begin(), bag.end(),
                                     [&](const Label &other) { return other.dominates(label); });
          // This label is dominated, it can be ignored.
          if (search != bag.end()) {
            continue;
          }

          // Does this label dominate anything in the bag already? If so, remove those labels.
          auto d_it = std::partition(bag.begin(), bag.end(),
                                     [&](const Label &other) { return !label.dominates(other); });
          for (auto it = d_it; it != bag.end(); ++it) {
            deleted_labels.insert((*it).label_id);
          }
          bag.erase(d_it, bag.end());
          bag.push_back(label);
          label_queue.push(label);
        }
      }
    }
  }

  return build_result_string(shortest_path_tree, source_node_id, target_node_id);
}

std::string Router::build_result_string(const NodeToLabelMap &shortest_path_tree,
                                        NodeID source_node_id, NodeID target_node_id) {
  std::vector<std::string> names;
  std::vector<double> charge_times;

  Label curr = shortest_path_tree.at(target_node_id);
  while (curr.node_id != source_node_id) {
    names.push_back(network_.at(curr.node_id).name);
    charge_times.push_back(ms_to_hours(curr.charge_time));
    curr = shortest_path_tree.at(curr.parent);
  }

  // The last charge time is how long we charged at the source, always 0.
  charge_times.pop_back();

  // Construct results in reverse because names/charge_times are currently
  // kept in target -> source order, but spec asks for source -> target.
  std::string result = network_.at(source_node_id).name;
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
  const row &source_row = network_.at(source);
  const row &dest_row = network_.at(dest);
  return haversine_dist(source_row.lat, source_row.lon, dest_row.lat, dest_row.lon);
}
