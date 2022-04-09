# Network

Class is used to store all of the network components. 
It includes instances of: 
[```Configuration```](/developer_manual/class_description/configuration/configuration.md), 
[```RoutingAlgorithm```](/developer_manual/class_description/routing/routing_algorithm.md), 
[```SelectionStrategy```](/developer_manual/class_description/selection/selection_strategy.md), 
[```TrafficManager```](/developer_manual/class_description/configuration/traffic_manager.md), 
[```FlitTracer```](/developer_manual/class_description/metrics/flit_tracer.md), 
[```SimulationTimer```](simulation_timer.md), 
clock signal, reset signal, array of 
[```Tile```](tile.md)s.

### Constructor
```c++
Network(const Configuration& config, const SimulationTimer& timer, sc_module_name = "NoC")
```
Initializes constant fields of class and calls InitBase meghod.

### Method
```c++
void InitBase()
```
Used for initialization of non-constant class fields, 
creation and configuration of [```Tile```](tile.md) instances and 
connection of [```Router```](router.md) instances according 
to the given network topology graph.
