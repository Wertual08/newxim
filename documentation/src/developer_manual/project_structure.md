# Project structure



Source code organization follows area of responsibility for components.<br>
Project directories tree:
```
- src                   # source code files
  - Configuration       # classes for initial simulator configuration
    - Graph             # classes for working with graphs
    - TrafficManagers   # traffic manager implementations
  - Data                # basic data structures
  - Hardware            # hardware elements of the network
  - Metrics             # metrics aggregation and display
  - Routing             # routing algorithm implementations
  - Selection           # selection strategy implementations  
```