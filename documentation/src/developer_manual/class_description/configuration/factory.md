# Factory

### Constructor 
```c++ 
Factory(const Configuration& cfg)
```
Accepts constant reference to configuration

### Method 
```c++
std::unique_ptr<RoutingAlgorithm> MakeAlgorithm() const
```
Returns instance of routing algorithm corresponding to the name specified in the configuration.

### Method 
```c++
std::unique_ptr<SelectionStrategy> MakeStrategy() const;
```
Returns instance of selection strategy corresponding to the name specified in the configuration.

### Method 
```c++
std::unique_ptr<TrafficManager> MakeTraffic() const;
```
Returns instance of traffic manager corresponding to the name specified in the configuration.
