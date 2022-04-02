# Topology parameters



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
```Gk``` - generators of the circulant<br><br>

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