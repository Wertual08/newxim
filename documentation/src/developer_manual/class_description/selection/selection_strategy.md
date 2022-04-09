# SelectionStrategy

Abstract class used as base class for the selection strategy implementation.

### Method
```c++
Connection Apply(const Router& router, const Flit& flit, const std::vector<Connection>& directions)
```
Implementation must select one or none of the given 
[```Connection```](/developer_manual/class_description/hardware/connection.md)s 
based on the 
[```Router```](/developer_manual/class_description/hardware/router.md) 
and 
[```Flit```](/developer_manual/class_description/data/flit.md) 
instances

