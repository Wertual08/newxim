# General simulator structure



In general, simulator consists of four stages:
- Configuration
- Setup
  - RoutingTable
  - Graph
  - Factory
    - TrafficManager
    - SelectionStrategy
    - RoutingAlgorithm
- Network
- GlobalStats

![General Newxim structure](/images/general_structure.svg)

Those stages are initialized and executed sequentially through the simulation life cycle[link] 
and determine the behavior in the following steps.