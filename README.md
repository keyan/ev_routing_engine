# Tesla Routing Problem

This program finds a single-source-single-destination shortest path minimizing only for total time of travel.

## Algorithmic Approaches

1. Constrained shortest paths
    - https://ad-publications.cs.uni-freiburg.de/IWCTS_constrainedev_S_2012.pdf
    - makes a simplifying assumption that battery is always fully-charged at a station
    - and that the time to full charge is constant (doesn't depend on current state of charge [soc])
1. CHarge method
    - https://arxiv.org/pdf/1910.09812.pdf

## My approach

### Pre-processing
1. Build a straightforward network graph
1. Prune edges that are longer than the maximum car charger length (320km)

### Queries
1. Start at the origin
1. Do a standard dijkstra's with the following changes
1. When examining a node, 
