# Flit

Storage class (struct) used to represent flit instance for the simulation.

### Field
```c++ 
std::uint64_t id
```
Global unique id of flit.

### Filed
```c++
int src_id
```
Id of the source node.

### Field
```c++
int dst_id
```
Id of the destination node.

### Field
```c++
int port_in
```
Number of router port flit came in (changes on each hop).

### Field
```c++
int port_out
```
Number of router port flit came from (changes on each hop).

### Field
```c++
int vc_id
```
Id of current flit`s virtual channel.

### Field
```c++
FlitType flit_type
```
Type of the flit. 
Can be one or several of: ```None```, ```Head```, ```Body```, ```Tail```

### Field
```c++
int sequence_no
```
Sequence number of flit in packet.

### Field
```c++
int sequence_length
```
Total number of flits in parent packet.

### Field
```c++
double timestamp = -1
```
Timestamp of flit creation.

### Field
```c++
double accept_timestamp
```
Timestamp of flit injection into network.

### Field
```c++
int hop_no
```
Current number of hops (increments on each hop).
