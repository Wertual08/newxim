# RoutingTable

Stores routing table for the simulation as 3D array. 
First index is a source node, second index is a destination node, third index is a variant of next node.

### Constructor 
```c++ 
RoutingTable()
```
Creates empty instance of routing table.

### Methods 
```c++
bool Load<name>(const Graph& graph)
```
Generate routing table from graph with specified algorithm.


### Method 
```c++
void Adjust(const Graph &src_graph, const Graph &dst_graph)
```
Adjust routing table from src_graph to dst_graph. Can be used to make child 
routing table from existing one for src_graph. Works only if dst_graph is subgraph for src_graph.

### Method 
```c++
void Promote(const Graph &graph)
```
Promote routing table indices upper on size of each node from the given graph. 
Can be used for combining routing tables of two merged graphs.
