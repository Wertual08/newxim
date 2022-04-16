# ProcessorQueue

This class objects are used as immitation of [```Processor```](processor.md) queue.
Memory economy of it is achieved by not actually storing [```Packet```](../data/packet.md)s.
Instead, it just stores numbers, representing sizes, timestamps and 
other parameters needed to estimate metrics of actual [```Packet```](../data/packet.md)s.

### Method
```c++
bool UpdateRequired() const
```
Returns true if update method call required

### Method
```c++
void UpdateFrontPacket(std::int32_t src_id, std::int32_t dst_id, std::int32_t size)
```
Updates the front [```Packet```](../data/packet.md) 
of the [```ProcessorQueue```](processor_queue.md)

### Method
```c++
void Push(double time_stamp)
```
Immitates push of [```Packet```](../data/packet.md) 
with specified timestamp to the [```ProcessorQueue```](processor_queue.md)

### Method
```c++
void Pop()
```
Immitates pop of [```Packet```](../data/packet.md) 
from the [```ProcessorQueue```](processor_queue.md)

### Method
```c++
Packet& Front()
```
Returns the current [```Packet```](../data/packet.md) of the [```ProcessorQueue```](processor_queue.md)

### Method
```c++
bool Empty() const
```
Retuns true if the [```ProcessorQueue```](processor_queue.md) is empty

### Method
```c++
std::size_t Size() const
```
Returns number of the [```Packet```](../data/packet.md)s in the [```ProcessorQueue```](processor_queue.md)