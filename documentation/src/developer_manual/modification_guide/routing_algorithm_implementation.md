# Routing algorithm implementation


This section will give you an example of implementing XY routing algorithm for mesh.<br>
First of all, you need to decide what data will be used by algorithm. 
XY routing algorithm must know width and height of the mesh to calculate 
node id from coordinates and vice versa. 
Secondly, it need access to the graph of the network for extracting link 
indices from it because each link index correlates with specific router port number.

```c++
class RoutingExampleMeshXY : public RoutingAlgorithm {
private:
	const std::int32_t mesh_w, mesh_h;
	const Graph& graph;

public:
	RoutingExampleMeshXY(std::int32_t w, std::int32_t h, const Graph& g) :
		mesh_w(w), mesh_h(h), graph(g) {
	}
};
```


Class ```RoutingExampleMeshXY``` have all required private fields for algorithm to operate 
and constructor for initialization which will be used later.
Next, we implement several helper methods:
```c++
std::int32_t GetXFromID(std::int32_t id) const {
	return id % mesh_w;
}
std::int32_t GetYFromID(std::int32_t id) const {
	return id / mesh_w;
}
std::int32_t GetIDFromXY(std::int32_t x, std::int32_t y) const {
	if (x >= mesh_w) {
		x = 0;
	} else if (x < 0) {
		x = mesh_w - 1;
	}
	if (y >= mesh_h) {
		y = 0;
	} else if (y < 0) {
		y = mesh_h - 1;
	}
	return x + y * mesh_w;
}
std::vector<std::int32_t> GetLinksTo(std::int32_t id, std::int32_t x, std::int32_t y) const {
	return graph[id].links_to(GetIDFromXY(x, y));
}
```

Those methods will be used for conversions between coordinates and node id. ```GetLinksTo(...)``` method accepts id of current node and coordinates of the next node. It returns array of link indices in graph. Those indices can be used as target router ports.


Now, it is time to implement routing method.

```c++
void Route(const Router& router, const Flit& flit, std::vector<Connection>& result) const override {
	std::int32_t id = router.LocalID;
	std::int32_t x = GetXFromID(router.LocalID);
	std::int32_t y = GetYFromID(router.LocalID);
	std::int32_t dx = GetXFromID(flit.dst_id) - x;
	std::int32_t dy = GetYFromID(flit.dst_id) - y;

	if (dx > 0) {
		for (std::int32_t l : GetLinksTo(id, x + 1, y)) {
			result.push_back({ l, flit.vc_id });
		}
	} else if (dx < 0) {
		for (std::int32_t l : GetLinksTo(id, x - 1, y)) {
			result.push_back({ l, flit.vc_id });
		}
	} else if (dy > 0) {
		for (std::int32_t l : GetLinksTo(id, x, y + 1)) {
			result.push_back({ l, flit.vc_id });
		}
	} else if (dy < 0) {
		for (std::int32_t l : GetLinksTo(id, x, y - 1)) {
			result.push_back({ l, flit.vc_id });
		}
	}
}
```

```Route(...)``` is marked as override because it implements abstract method of base 
[```RoutingAlgorithm```](../class_description/routing/routing_algorithm.md) class. 
It accepts references to router, flit and vector of result connections instances. 
Reference to router can be used in more complex algorithms which relies on current network state. 
For now, we only require ```LocalID``` as it represents the current node. Flit instance have some useful 
information such as destination node id and current virtual channel id. Result vector consists of connections. 
Each connection is just a pair of port number and virtual channel index. 
At first step, all variables, required for algorithm to operate are calculated, then it chose direction based on dx and dy. 
Links for the chosen direction are written in result vector along with current flit virtual channel id (XY algorithm have no reasons to change it).
Now algorithm is fully functional, but it needs to be registered in the 
[```Factory```](../class_description/configuration/factory.md). 
It is required because at configuration step, 
[```Factory```](../class_description/configuration/factory.md) 
needs to find specific algorithm implementation by its name. 
Found algorithm implementation will then be injected into each router of the network.
Function: 
```c++ 
std::unique_ptr<RoutingAlgorithm> Factory::MakeAlgorithm()
``` 
is used to inject routing algorithm instance. 
To register new routing algorithm only single line needs to be added in the function body.

```c++
std::unique_ptr<RoutingAlgorithm> Factory::MakeAlgorithm() const
{
	if (config.RoutingAlgorithm() == "EXAMPLE_MESH_XY") return std::make_unique<RoutingExampleMeshXY>(config.DimX(), config.DimY(), config.TopologyGraph());
    //...
	throw std::runtime_error("Configuration error: Invalid routing algorithm [" + config.RoutingAlgorithm() + "].");
}
```

It checks algorithm name, provided by configuration and if it equals to your algorithm name, returns unique instance of algorithm implementation.

