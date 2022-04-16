# Simulation life cycle

When you run program, sc_main entry point is called. It initializes 
[```Configuration```](../class_description/configuration/configuration.md)
instance and 
creates 
[```SimulationTimer```](../class_description/hardware/simulation_timer.md), 
[```GlobalStats```](../class_description/metrics/global_stats.md)
and 
[```ProgressBar```](../class_description/metrics/progress_bar.md)
objects. 
Then it rises reset signal and run simulation for reset time, specified in config. 
Next, reset signal is lowered to zero and simulation is run for required time. 
When the simulation completes, 
[```GlobalStats```](../class_description/metrics/global_stats.md)
object is serialized to the standard output stream of program.<br>

Each simulation cycle consists of three unordered stages:

- #### Flit generation in processors
  Each processor asks the traffic manager for permission to spawn packet. If packet should be created, router adds it to the packet queue. Queue is not actually holding every packet instance for memory efficiency. Instead, it just remembers how many packets it should spawn and current generated packet. Current packet is hold until each of its flits are sent. Then, the next packet is generated if it exists in queue. After generating packet, processor checks if it tries to send next flit of the queue to the router. On success flit is removed from queue.
- #### Flit consumption
  Processors check for incoming flits and record statistics on each received one.
- #### Flits routing
  Router loop through each port and tries to calculate the output port. If destination port is available, it reserves source port to destination port and place that record in reservation table. Then, for each record in reservation table flit transmission is performed.

Many operations, such flit consumption and routing records some information to the stats object. After simulation those objects are aggregated in global stats and printed as a result of simulation.
