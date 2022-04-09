# Simulation output

Simulation output depends on [configuration file](metrics_options.md). 
Basically, it consists of general metrics section and configurable sections.
General metrics can be shown in json or plain text format.

## General metrics
- #### Total produced flits
  The number of flits produced by projessors during the simulation
- #### Total accepted flits
  The number of flits received in the network from processors during the simulation
- #### Total received flits
  The number of flits received by processors during the simulation
- #### Network production (flits/cycle)
  Average number of flits produced by processors per cycle
- #### Network acceptance (flits/cycle)
  Average number of flits received in the network from processors per cycle
- #### Network throughput (flits/cycle)
  Average number of flits delivered by the network to processors per cycle
- #### IP throughput (flits/cycle/IP)
  Average number of flits delivered by the network to each processor per cycle
- #### Last time flit received (cycles)
  Last cycle where any of flits was received by any of processors
- #### Max buffer stuck delay (cycles)
  Maximum time in cycles between pulling outs of flits from any of buffers
- #### Max time flit in network (cycles)
  Maximum time in cycles flit took to be delivered
- #### Total received packets
  The number of complete packets, received by processors during the simulation
- #### Total flits lost
  The number of flits, lost in network (should be zero if simulation is working correctly)
- #### Global average delay (cycles)
  Average delay in cycles between packet creation and consumption
- #### Max delay (cycles)
  Maximum delay in cycles between packet creation and consumption
- #### Average buffer utilization:
  Average flit slots utilized among all buffers and cycles


## Configurable sections
- #### Flit trace
  Reports path of each flit through the network
- #### Buffer statuses
  Reports metrics for each buffer and flits left in them before the simulation ended
- #### Flits distribution
  Reports number of sent and received flits for each processor