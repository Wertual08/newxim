# Algorithms configuration


#### 1. Routing algorithm selection
```yml
routing_algorithm: <type>
```
##### ```TABLE_BASED```             - table based routing
##### ```MESH_XY```                 - XY routing algorithm for mesh
##### ```MESH_WEST_FIRST```         - West First algorithm for mesh
##### ```MESH_O1TURN```             - O1TURN algorithm for mesh
##### ```MESH_XY_YX```              - XY-YX algorithm for mesh
##### ```MESH_NEGATIVE_FIRST```     - Negative First algorithm for mesh
##### ```MESH_NORTH_LAST```         - North Last algorithm for mesh
##### ```MESH_ODD_EVEN```           - Odd-Even algorithm for mesh
##### ```TORUS_CLUE```              - CLUE algorithm for torues
##### ```SUBNETWORK```              - table based algorithm which also uses physical subnetwork with permission for packets to leave subnetwork
##### ```FIXED_SUBNETWORK```        - table based algorithm which also uses physical subnetwork without permission for packets to leave subnetwork
##### ```VIRTUAL_SUBNETWORK```      - table based algorithm which also uses virtual subnetwork with permission for packets to leave subnetwork
##### ```FIT_VIRTUAL_SUBNETWORK```  - table based algorithm which also uses virtual subnetwork without permission for packets to leave subnetwork and buffers load control


#### 2. Strategy of selecting one of ports and virtual channels provided by the routing algorithm
```yml
selection_strategy: <type>
```
##### ```RANDOM```              - random port selection
##### ```BUFFER_LEVEL```        - less loaded port selection
##### ```KEEP_SPACE```          - less loaded port selection with control of network load
##### ```RANDOM_KEEP_SPACE```   - random port selection with control of network load
##### ```CIRCULANT_RING_SPLIT```          - ring-split selection strategy for circulants
##### ```CIRCULANT_VIRTUAL_RING_SPLIT```  - ring-split selection strategy with virtual channels for circulants


#### 3. Routing table configuration
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
##### Custom routing table format
```yml
[
    [X, Y, Z, [A, B, C], ...],
    [U, V, W, ...]
    ...
]
```
Each row in table corresponds to one router. Each value in 
that row represents port or id of the router (or array of those) 
in which packet must be sent on routing stage.


#### 4. Routing table format
```yml
routing_table_id_based: <true/false>
```


#### 5. Type of traffic distribution
```yml
traffic_distribution: <type>
```
##### ```TRAFFIC_RANDOM``` - random traffic distribution<br>
##### ```TRAFFIC_HOTSPOT``` - hotspot traffic distribution<br>
##### ```TRAFFIC_TABLE_BASED``` - traffic distribution based on table from file


#### 6. Configuration of hotspots
```yml
traffic_hotspots: [[N, S, R], ...]
```
##### ```N``` - node index<br>
##### ```S``` - send probability multiplier<br>
##### ```R``` - receive probability multiplier


#### 7. Traffic table file path
```yml
traffic_table_filename: <path> 
```