# Relay

Represents port with flit buffers. Must be connected with another relay. Used for transmitting flits between routers and processors.

### Method
```c++
void SetLocalID(std::int32_t id)
```
Sets owner [```Router```](router.md) id for the [```Relay```](relay.md)

### Method
```c++
void SetVirtualChannels(std::size_t vcs)	
```
Sets number of virtual channels

### Method
```c++
std::size_t Size()
```
Returns number of virtual channels

### Method
```c++
void Bind(Relay& r)
```
Pairs current [```Relay```](relay.md) with provided one

### Method
```c++
bool Bound()
```
Returns true if array is bound to another [```Relay```](relay.md)

### Method
```c++
void Disable()
```
Allows to leave [```Relay```](relay.md) unbound (binds it to itself)

### Method
```c++
void Reset()
```
Performs reset

### Method
```c++
void Update()
```
Performs per-cycle update

### Method
```c++
bool CanSend(const Flit &flit)
```
Returns true if [```Flit```](/developer_manual/class_description/data/flit.md) can be send

### Method
```c++
bool CanSend(std::size_t vc)
```
Returns true if [```Flit```](/developer_manual/class_description/data/flit.md) can be send through specified virtual channel

### Method
```c++
bool Send(Flit flit)
```
Tries to send [```Flit```](/developer_manual/class_description/data/flit.md). Returns true on success

### Method
```c++
bool CanReceive()
```
Returns true if [```Flit```](/developer_manual/class_description/data/flit.md) can be received

### Method
```c++
Flit Receive()
```
Returns [```Flit```](/developer_manual/class_description/data/flit.md) if receive succeed, 
otherwise returns invalid [```Flit```](/developer_manual/class_description/data/flit.md) instance

### Method
```c++
std::size_t GetFreeSlots(std::size_t vc)
```
Returns number of free slots in the [```Buffer```](buffer.md) for given virtual channel

### Method
```c++
Flit Front()
```
Returns front [```Flit```](/developer_manual/class_description/data/flit.md) 
from the [```Buffer```](buffer.md) of current virtual channel

### Method
```c++
void Skip()
```
Skips current virtual channel

### Method
```c++
Flit Pop()
```
Removes [```Flit```](/developer_manual/class_description/data/flit.md) from the current virtual channel [```Buffer```](buffer.md)

### Method
```c++
Buffer& operator[](std::size_t i)
```
Overloaded operator for accessing [```Buffer```](buffer.md) of specified virtual channel
