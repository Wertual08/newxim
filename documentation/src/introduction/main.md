# Introduction
The given software provides modeling functionality for 
networks on chip with [custom topology](../user_manual/topology_parameters.md), 
[routing algorithm and selection strategy](../user_manual/algorithms_configuration.md). 
There is simple programming interface for introducing new modeling capabilities. 
The project was originally inspired by the [Noxim](https://github.com/davidepatti/noxim), 
but the goal of flexibility led to complete rework of the whole simulator. 
Modeling of signals is driven by the [SystemC](https://www.accellera.org/downloads/standards/systemc) library 
which allows cycle accurate communication between components of the network. 
The main purpose of development was to create universal toolset for high level modeling, 
evaluation and comparison of different network topologies. 
Such properties as performance and ease of modifications became the basis of the Newxim.
