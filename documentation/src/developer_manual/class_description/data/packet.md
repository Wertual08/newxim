# Packet

Storage class (struct) used to represent packet instance for the simulation.

### Field
```c++ 
std::int32_t src_id
```
ID of the source node

### Field
```c++ 
std::int32_t dst_id
```
ID of the destination node

### Field
```c++ 
std::int32_t vc_id
```
ID of the virtual channel

### Field
```c++ 
double timestamp
```
Simulation time of packet creation

### Field
```c++ 
std::int32_t size
```
Number of flits in packet

### Field
```c++ 
std::int32_t flit_left
```
Flits in the packet that are left to send from [```Processor```](../../class_description/hardware/processor.md)