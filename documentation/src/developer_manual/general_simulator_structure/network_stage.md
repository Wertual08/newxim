# Network stage

On this stage whole simulation process happens. 
Simulation can be described as a state machine.
On each iteration, new simulation state is produced from the current state.
All calculations are done via ```Update``` methods bound to the clock signal.
During the simulation most of operational units record some metrics. 
Those will be used in the [final stage](global_stats_stage.md).