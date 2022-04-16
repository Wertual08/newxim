# SelectionStrategy

Abstract class used as base class for the selection strategy implementation.

### Method
```c++
Connection Apply(const Router& router, const Flit& flit, const std::vector<Connection>& directions)
```
Implementation must select one or none of the given 
[```Connection```](../hardware/connection.md)s 
based on the 
[```Router```](../hardware/router.md) 
and 
[```Flit```](../data/flit.md) 
instances

