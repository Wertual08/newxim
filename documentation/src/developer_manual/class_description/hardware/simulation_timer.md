# SimulationTimer

Class object provides access to current timestamps in different systems.


### Method
```c++
double ClockPeriod() const
```
Returns clock period

### Method
```c++
double SystemTime() const
```
Returns timestamp from the start of the simulation including reset time

### Method
```c++
double SimulationTime() const
```
Returns timestamp from the start of the simulation excluding reset time

### Method
```c++
double StatisticsTime() const
```
Returns timestamp from the start statistics aggregation

### Method
```c++
double ProductionTime() const
```
Returns timestamp when production of the 
[```Flit```](../data/flit.md)s 
in the [```Processor```](processor.md)s must be stopped

### Method
```c++
double Progress() const
```
Returns current simulation progress