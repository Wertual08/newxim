# Graph

Extends ```std::vector<GraphNode>``` and provides some methods for calculation of subgraph and path searching. 
Child classes ```<name>Graph``` provide constructors for the graph of given topology.


## GraphNode
Extends ```std::vector<std::int32_t>```, stores indices of nodes, connected to it.

### Method 
```c++
void push_back(std::int32_t id, std::int32_t ch = 1)
```
- Accepts node index and the number of required physical connections.
- Used to connect current node with node at specified index.

### Method 
```c++
std::vector<std::int32_t> links_to(std::int32_t id)
```
- Accepts node index.
- Returns indices of the links (corresponding to router ports) to specified node.
