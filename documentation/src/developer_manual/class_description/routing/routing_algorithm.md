# RoutingAlgorithm

Abstract class used as base class for routing algorithm implementation.


### Method
```c++ 
void Route(const Router& router, const Flit& flit, std::vector<Connection>& result)
```
Implementation must provide an array of 
[```Connection```](/developer_manual/class_description/hardware/connection.md)s 
based on given 
[```Router```](/developer_manual/class_description/hardware/router.md) 
and 
[```Flit```](/developer_manual/class_description/data/flit.md)
instances
