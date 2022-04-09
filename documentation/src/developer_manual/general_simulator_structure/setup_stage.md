# Setup stage



On the setup 
[```RoutingTable```](/developer_manual/class_description/configuration/routing_table.md), 
[```Graph```](/developer_manual/class_description/configuration/graph.md)
and 
[```Factory```](/developer_manual/class_description/configuration/factory.md)
objects are built from configuration.
[```Graph```](/developer_manual/class_description/configuration/graph.md)
object will be used as a network topology, while the 
[```Factory```](/developer_manual/class_description/configuration/factory.md)
serves as a provider
of the 
[```TrafficManager```](/developer_manual/class_description/configuration/traffic_manager.md), 
[```SelectionStrategy```](/developer_manual/class_description/selection/selection_strategy.md) 
and 
[```RotuingAlgorithm```](/developer_manual/class_description/routing/routing_algorithm.md) 
instances for 
network processors and routers. 
[```Factory```](/developer_manual/class_description/configuration/factory.md)
is also used for [registering](/developer_manual/modification_guide/main.md) 
your own algorithms for configuration.