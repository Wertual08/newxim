# Processor

Represents processing element of the network. It is used as source or destination for packets.

### Method
```c++
Packet& GetQueueFront()
```
Updates first 
[```Packet```](../data/packet.md) 
if it is required and returns first packet from the 
[```ProcessorQueue```](processor_queue.md). 

### Method
```c++
void ReceiveFlit(Flit flit)
```
Updates statistics for received [```Flit```](../data/flit.md)s.

### Method
```c++
void SendFlit(Flit flit)
```
Updates statistics for sent [```Flit```](../data/flit.md)s.

### Method
```c++
Flit NextFlit()
```
Returns next [```Flit```](../data/flit.md) of current front packet of the [```ProcessorQueue```](processor_queue.md).

### Method
```c++
void PopFlit()
```
Removes 
[```Flit```](../data/flit.md) 
from the current 
[```Packet```](../data/packet.md). 
If there is no 
[```Flit```](../data/flit.md)s 
left in the current [```Packet```](../data/packet.md), 
removes [```Packet```](../data/packet.md) from the [```ProcessorQueue```](processor_queue.md).

### Constructor
```c++
Processor(const SimulationTimer& timer, std::int32_t id,
	std::int32_t min_packet_size, std::int32_t max_packet_size)
```
Creates new instance of the [```Processor```](processor.md).

### Method
```c++
void SetTrafficManager(const TrafficManager& traffic)
```
Sets [```TrafficManager```](../configuration/traffic_manager.md) pointer to given instance.

### Method
```c++
void SetFlitTracer(FlitTracer& tracer)
```
Sets [```FlitTracer```] ponter to given tinstance.

### Method
```c++
void Update()
```
Performs reset, receive and transmit operations.

### Method
```c++
void TXProcess()
```
Performs transmitting operations for current cycle.

### Method
```c++
void RXProcess()
```
Performs receiving operations for current cycle.
