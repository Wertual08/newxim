# Setup stage



On the setup ```RoutingTable```[link], ```Graph```[link] and ```Factory```[link] objects are built from configuration.
```Graph```[link] object will be used as a network topology, while the ```Factory```[link] serves as a provider
of the ```TrafficManager```[link], ```SelectionStrategy```[link] and ```RotuingAlgorithm``` instances for 
network processors and routers. ```Factory```[link] is also used for registering[link] your own algorithms for configuration