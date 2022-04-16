# General simulator structure



In general, simulator consists of four stages:
- [Configuration](configuration_stage.md)
- [Setup](setup_stage.md)
  - RoutingTable
  - Graph
  - Factory
    - TrafficManager
    - SelectionStrategy
    - RoutingAlgorithm
- [Network](network_stage.md)
- [GlobalStats](global_stats_stage.md)

![General Newxim structure](../../images/general_structure.svg)

Those stages are initialized and executed sequentially through the [Simulation life cycle](../simulation_life_cycle/main.md) 
and determine the behavior in the following steps.