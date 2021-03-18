# Tesla Routing Problem

This program finds a single-source-single-destination shortest path minimizing only for total time of travel.

## Building and Usage

To build run:

```
make
```

## Approach

I created an algorithmic approach based around a variant of Dijkstra's algorithm by using the observation that this problem resembles several similar path routing problems such as constrained shortest path and bicriteria shortest path.

Briefly, the idea is this:

First create the overall graph:
1. Construct an adjacency list representation of the network
2. Prune all edges that are infeasible, that is the trip cannot be made even with a full battery

To Perform a search, do a standard Dijkstra's except:
- instead of pushing nodes/vertices into a priority queue as normal, we push a "Label"
- "Labels" can be thought of as subdivisons of nodes in the graph, each node has a bag of labels representing every Pareto optimal way to reach that node from another node, as well as what the resulting battery state would be if arriving at that node using that Label, and how much charging time would be required at the parent node
- Instead of "relaxing" edges as we do in Dijkstra's, we create a group of candidate Labels, then check if they can dominate or are dominated by any existing Labels in the bag of each neighboring node
- Domination checking is neccesary because we care about both travel time and resulting battery state for each Label used
- The search terminates as usual when the destination/target station is added to the shortest path tree

A key simplifying point is that although battery state is continuous, in this model we need only consider doing one of three actions when arriving at a charging station:
1. Travel to the next station without charging at all
2. Do a full recharge
3. Only chage enough to get to the next station
