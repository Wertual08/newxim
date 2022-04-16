# Selection strategy implementation

This section describes implementation of custom selection strategy.<br>


```c++
class SelectionExampleRandom : public SelectionStrategy 
{
public:
	Connection Apply(const Router& router, const Flit& flit, const std::vector<Connection>& directions) const override
    {
        if (directions.size() == 0) {
            return Connection();
        } else {
            return directions[rand() % directions.size()];
        }
    }
};
```

```Apply(...)``` is marked as override because it implements abstract method of base 
[```SelectionStrategy```](../class_description/selection/selection_strategy.md)
class. 
It accepts references to router, flit and vector of direction instances. 
References to router and flit can be used in more complex selection 
strategies which relies on current network state. 
Current example just selects random connection instance and returns it.
If there is not connections passed to ```Apply``` function, it just returns invalid conneciton instace.
Function: 
```c++ 
std::unique_ptr<SelectionStrategy> Factory::MakeStrategy()
``` 
is used to inject routing algorithm instance. 
To register new routing algorithm only single line needs to be added in the function body.

```c++
std::unique_ptr<SelectionStrategy> Factory::MakeStrategy() const
{
	if (config.RoutingAlgorithm() == "EXAMPLE_RANDOM") return std::make_unique<SelectionExampleRandom>();
    //...
	throw std::runtime_error("Configuration error: Invalid selection strategy [" + config.SelectionStrategy() + "].");
}
```

It checks selection strategy name, provided by configuration and if it equals to your selection 
strategy name, returns unique instance of the selection strategy implementation.

