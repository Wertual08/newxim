# Router

Represents main network unit, which is responsible for routing 
[```Flit```](../data/flit.md) s over the network.

### Method
```c++
void Reservation(std::int32_t in_port)
```
Performs reservation process for given port if it has head 
[```Flit```](../data/flit.md)  in it.

### Method
```c++
void Update()
```
Calls per-cycle update operations.

### Method
```c++
Connection FindDestination(const Flit& flit)
```
Computes target connection for given 
[```Flit```](../data/flit.md) 
via routing algorithm and selection strategy.

### Method
```c++
bool Route(std::int32_t in_port, Connection dst)
```
Sends packet from given input port to output port via specified virtual channel.

### Method
```c++
virtual void TXProcess()
```
Performs per-cycle transmitting operations.
