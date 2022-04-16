# FlitTracer

Class instance is used to trace [```Flit```](../data/flit.md)s paths through the network.

### Constructor
```c++
FlitTracer(const SimulationTimer& timer, double story_start, double story_end)
```
Initializes new [```FlitTracer```](flit_tracer.md) instance

### Method
```c++
void Register(Flit& flit)
```
Registers [```Flit```](../data/flit.md) in tracer

### Method
```c++
void Remember(const Flit& flit, std::int32_t id)
```
Remembers [```Flit```](../data/flit.md) location

### Method
```c++
friend std::ostream& operator<<(std::ostream& os, const FlitTracer& tracer)
```
Overloaded operator to print traces to the output stream