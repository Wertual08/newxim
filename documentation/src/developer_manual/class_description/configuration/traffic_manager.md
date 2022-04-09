# TrafficManager

Abstract class, used as base class for traffic manager implementations.

### Abstract method
```c++ 
bool FirePacket(std::int32_t from, double time)
```
Implementation must decide if packet should be fired from processor with given index at given time.

### Abstract method
```c++ 
std::int32_t FindDestination(std::int32_t from)
```
Implementation must provide selection of the destination node index from the given node index.
