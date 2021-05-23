# NewXim
Network on Chip Simulator

This project is dedicated to cycle-accurate NoCs 
simulation and based on SystemC library. Main 
purpose of the project is to provide tools for 
analyzing NoCs with custom topologies and in 
particular Circulant topologies. There are 
several implemented topologies and potion to 
pecify custom one.

## User manual
Simulation configuration is done with *.yml 
file where parameters can be specified. 
Configuration file is selected via ```-config``` 
command line option (default is config.yml). 
Any parameter specified can be overridden with 
eponymous command line option.


### Parameters description
#### 1. Topology configuration
```yml 
topology: <type>
```
Possible values:<br>

##### ```CIRCULANT``` - circulant topology<br>
```yml 
topology_args: [N, G1, G2, …]
```
```N``` - number of nodes<br>
```Gk``` - generators of circulant<br><br>

##### ```MESH``` - mesh topology
```yml 
topology_args: [W, H]
```
```W``` - number of nodes horizontally<br>
```H``` - number of nodes vertically<br><br>

##### ```TORUS``` - torus topology
```yml 
topology_args: [W, H]
```
```W``` - number of nodes horizontally<br>
```H``` - number of nodes vertically<br><br>

##### ```TREE``` - tree topology
```yml 
topology_args: [N, C]
```
```N``` - number of nodes<br>
```C``` - maximum number of child nodes<br><br>

##### ```CUSTOM``` - custom topology
```yml 
topology_args: [
  [N00, N01, ...],
  [N10, N11, ...],
  ...
]
```
```Nab``` - index of node b which is connected to node a<br>


#### 2. Number of physically allocated channels
```yml 
topology_channels: <count>
```


#### 3. Number of virtually allocated channels
```yml 
virtual_channels: <count>
```

#### 4. Subtopology type
```yml
subtopology: <type>
```
```NONE``` - no subtopology<br>
```TREE``` - spanning tree with minimum possible Wiener index


#### 5. Subnetwork links allocation type
```yml
subnetwork: <type>
```
```NONE``` - no subnetwork<br>
```VIRTUAL``` - virtually allocated links<br>
```PHYSICAL``` - physically allocated links<br>


#### 6. Order of port buffers update
```yml
update_sequence: <type>
```
##### ```DEFAULT``` - rund-robin update order
##### ```[P1, P2, ...]``` - fixed update order of ports


#### 7. Number of flit slots per buffer
```yml
buffer_depth: <depth>
```


#### 8. Minimum length in flits of generated packets
```yml
min_packet_size: <size>
```


#### 9. Maximum length in flits of generated packets
```yml
max_packet_size: <size>
```


#### 10. Control injection rate in number of flits instead of packets
```yml
flit_injection_rate: <true/false>
```


#### 11. Control scaling of generated packets / flits with number of nodes
```yml
scale_with_nodes: <true/false>
```


#### 12. Probability of packet / flit generation for node per every simulation iteration
```yml
packet_injection_rate: <rate>
```


#### 13. Routing algorithm selection
```yml
routing_algorithm: <type>
```
##### ```TABLE_BASED``` - table based routing<br>
##### ```MESH_XY``` - XY routing algorithm for mesh<br>
##### ```SUBNETWORK``` - table based algorithm which also uses physical subnetwork with permission for packets to leave subnetwork<br>
##### ```FIXED_SUBNETWORK``` - table based algorithm which also uses physical subnetwork without permission for packets to leave subnetwork<br>
##### ```VIRTUAL_SUBNETWORK``` - table based algorithm which also uses virtual subnetwork with permission for packets to leave subnetwork<br>
##### ```RING_SPLIT``` - ring-split routing algorithm for circulants which also uses routing table<br>
##### ```VIRTUAL_RING_SPLIT``` - ring-split routing algorithm with virtual channels for circulants which also uses routing table


#### 14. Strategy of selecting one of ports provided by routing algorithm
```yml
selection_strategy: <type>
```
##### ```RANDOM``` - random port selection<br>
##### ```BUFFER_LEVEL``` - less loaded port selection<br>
##### ```KEEP_SPACE``` - less loaded port selection with control of network load
##### ```RANDOM_KEEP_SPACE``` - random port selection with control of network load


#### 15. Routing table configuration
```yml
routing_table: <type>
```
##### ```DIJKSTRA``` - routing table is based on Dijkstra algorithm<br>
##### ```UP_DOWN``` - routing table is based on up-down algorithm<br>
##### ```MESH_XY``` - routing table is based on XY algorithm for mesh<br>
##### ```CIRCULANT_PAIR_EXCHANGE``` - routing table is based on pair-exchange algorithm for circulant<br>
##### ```CIRCULANT_MULTIPLICATIVE``` - routing table is based on routing algorithm for multiplicative cicrulant<br>
##### ```CIRCULANT_CLOCKWISE``` - routing table is based on clockwise algorithm for circulant<br>
##### ```GREEDY_PROMOTION``` - routing table is based on greedy promotion algorithm<br>
##### ```[...]``` - just routing table


#### 16. Routing table format
```yml
routing_table_id_based: <true/false>
```


#### 17. Simulation random seed
```yml
rnd_generator_seed: <seed>
```


#### 17. Simulation random seed
```yml
rnd_generator_seed: <seed>
```


#### 18. Report simulation progress as progress bar
```yml
report_progress: <true/false>
```


#### 19. Report buffer statistics after simulation
```yml
report_buffers: <true/false>
```


#### 20. Report graph used in current simulation
```yml
report_topology_graph: <true/false>
```


#### 21. Report graph adjency matrix used in current simulation
```yml
report_topology_graph_adjency_matrix: <true/false>
```


#### 22. Report routin table used in current simulation
```yml
report_routing_table: <true/false>
```


#### 23. Report possible routes 
```yml
report_possible_routes: <true/false>
```


#### 24. Report routes metrics
```yml
report_routes_stats: <true/false>
```


#### 25. Report subgraph used in current simulation
```yml
report_topology_sub_graph: <true/false>
```


#### 26. Report subgraph adjency matrix used in current simulation
```yml
report_topology_sub_graph_adjency_matrix: <true/false>
```


#### 27. Report routin table for subgraph used in current simulation
```yml
report_sub_routing_table: <true/false>
```


#### 28. Report metrics for every cycle
```yml
report_cycle_result: <true/false>
```


#### 29. Report trace of flit routes
```yml
report_flit_trace: <true/false>
```


#### 30. Duration of simulation cycle in ps
```yml
clock_period_ps: <count>
```


#### 31. Duration of reset signal in cycles
```yml
reset_time: <count>
```


#### 32. Duration of simulation in cycles
```yml
simulation_time: <count>
```


#### 33. Duration of the period which is ignored in stats accumulation in cycles
```yml
stats_warm_up_time: <count>
```


#### 34. Type of traffic distribution
```yml
traffic_distribution: <type>
```
##### ```TRAFFIC_RANDOM``` - random traffic distribution<br>
##### ```TRAFFIC_HOTSPOT``` - hotspot traffic distribution<br>
##### ```TRAFFIC_TABLE_BASED``` - traffic distribu;tion based on table from file


#### 35. Configuration of hotspots
```yml
traffic_hotspots: [[N, S, R], ...]
```
##### ```N``` - node index<br>
##### ```S``` - send probability multiplier<br>
##### ```R``` - receive probability multiplier


#### 36. Traffic table file path
```yml
traffic_table_filename: <path> 
```
