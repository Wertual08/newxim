# Stats

Class objects are used to local metrics aggregation inside 
[```Router```](../hardware/router.md)s.


### Method
```c++
void FlitRouted(const Flit& flit);
```
Remembers stats for the routed [```Flit```](../data/flit.md)

### Method
```c++
void FlitReceived(std::int32_t relay, std::int32_t vc);
```
Remembers stats for the received [```Router```](../hardware/router.md)

### Method
```c++
void StartStuckTimer(std::int32_t relay, std::int32_t vc);
```
Starts timer for [```Buffer```](../hardware/buffer.md) stuck delay

### Method
```c++
void StopStuckTimer(std::int32_t relay, std::int32_t vc);
```
Stops timer for [```Buffer```](../hardware/buffer.md) stuck delay

### Method
```c++
void PushLoad(std::int32_t relay, std::int32_t vc, double load);
```
Remembers stats of buffer load

### Method
```c++
double GetMaxBufferStuckDelay(std::int32_t relay, std::int32_t vc);
```
Returns maximum stuck delay of the specified [```Buffer```](../hardware/buffer.md)

### Method
```c++
std::int32_t GetBufferFlitsReceived(std::int32_t relay, std::int32_t vc);
```
Returns number of flits received by specified [```Buffer```](../hardware/buffer.md) 

### Method
```c++
double GetMaxBufferStuckDelay();
```
Returns maximum stuck delay among all of the [```Buffer```](../hardware/buffer.md)s

### Method
```c++
double GetAverageBufferLoad(std::int32_t relay, std::int32_t vc) const;
```
Returns average load of the specified [```Buffer```](../hardware/buffer.md)

### Method
```c++
double GetAverageBufferLoad() const;
```
Returns average load among all of the [```Buffer```](../hardware/buffer.md)s

### Method
```c++
std::int32_t GetFlitsRouted() const;
```
Returns number of the routed [```Flit```](../data/flit.md)s