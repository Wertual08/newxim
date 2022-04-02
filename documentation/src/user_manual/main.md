# User manual

Software is provided as CLI. The only purpose of it is to simulate network with given parameters. 
Simulation is single threaded, but in most cases, you will require multiple of them, so you are 
able to run as many parallel instances as you want. All automatizations are done with other modules 
such as [NewximManager]. There are two ways of configuring simulation process. With config file or 
via command line arguments. Due to higher priority of command line arguments, any file configuration 
can be overwritten on program startup. Config file (by default “config.yml” and can be set via -config 
command line option) must be in yaml format and contains network, simulation and metrics properties.

## Command line arguments
Each config option have dedicatied command line argument.
To override option, just pass ```-<config option name> <config option value>```<br>
For example: ```-topology CIRCULANT -packet_injection_rate 0.2```