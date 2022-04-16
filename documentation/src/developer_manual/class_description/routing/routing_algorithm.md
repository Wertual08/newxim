# RoutingAlgorithm

Abstract class used as base class for routing algorithm implementation.


### Method
```c++ 
void Route(const Router& router, const Flit& flit, std::vector<Connection>& result)
```
Implementation must provide an array of 
[```Connection```](../hardware/connection.md)s 
based on given 
[```Router```](../hardware/router.md) 
and 
[```Flit```](../data/flit.md)
instances
