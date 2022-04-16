# Buffer

Buffer is used to store [```Flit```](../data/flit.md) s for one virtual channel.

### Method
```c++
void Reserve(std::size_t bms)
```
Reserves capacity for the [```Buffer```](buffer.md)

### Method
```c++
std::size_t GetCapacity()
```
Returns capacity of the [```Buffer```](buffer.md)

### Method
```c++
std::size_t GetFreeSlots()
```
Returns current number of free slots

### Method
```c++
bool Full()
```
Returns true if [```Buffer```](buffer.md) is full

### Method
```c++
bool Empty()
```
Returns true if [```Buffer```](buffer.md) is empty

### Method
```c++
void Clear()
```
Clears the [```Buffer```](buffer.md)

### Method
```c++
void Push(const Flit& flit)
```
Appends [```Flit```](../data/flit.md)  to the end of the [```Buffer```](buffer.md)

### Method
```c++
Flit Pop()
```
Remove and return front [```Flit```](../data/flit.md)  from the [```Buffer```](buffer.md)

### Method
```c++
Flit Front()
```
Returns front [```Flit```](../data/flit.md)  from the [```Buffer```](buffer.md)

### Method
```c++
std::int32_t Size()
```
Returns number of the [```Flit```](../data/flit.md)s in the [```Buffer```](buffer.md)

### Method
```c++
double GetOldest()
```
Returns timestamp of the oldest [```Flit```](../data/flit.md) in the [```Buffer```](buffer.md)

### Method
```c++
double GetOldestAccepted()
```
Returns accepted timestamp of the oldest [```Flit```](../data/flit.md) in the [```Buffer```](buffer.md)

### Method
```c++
double GetLoad()
```
Returns the share of [```Buffer```](buffer.md) load
