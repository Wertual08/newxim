# Setup stage



On the setup 
[```RoutingTable```](../configuration/routing_table.md), 
[```Graph```](../configuration/graph.md)
and 
[```Factory```](../configuration/factory.md)
objects are built from configuration.
[```Graph```](../configuration/graph.md)
object will be used as a network topology, while the 
[```Factory```](../configuration/factory.md)
serves as a provider
of the 
[```TrafficManager```](../configuration/traffic_manager.md), 
[```SelectionStrategy```](../selection/selection_strategy.md) 
and 
[```RotuingAlgorithm```](../routing/routing_algorithm.md) 
instances for 
network processors and routers. 
[```Factory```](../configuration/factory.md)
is also used for [registering](../modification_guide/main.md) 
your own algorithms for configuration.