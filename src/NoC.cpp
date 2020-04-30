/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the Network-on-Chip
 */

#include "NoC.h"
#include "routingAlgorithms/RoutingTableBased.h"
#include "selectionStrategies/SelectionRandom.h"
#include "selectionStrategies/SelectionBufferLevel.h"



NoC::NoC(const Configuration& config) : 
	clock("clock", GlobalParams::clock_period_ps, SC_PS), 
	BorderRelay("BorderRelay"), GRTable(config.GRTable())
{
	srand(config.RndGeneratorSeed());
	//std::cout << config.Topology() << '\n';
	//std::cout << config.GRTable() << '\n';

	// Check for traffic table availability
	if (config.TrafficDistribution() == TRAFFIC_TABLE_BASED)
		GTTable.load(config.TraceFilename().c_str());

	if (config.RoutingAlgorithm() == ROUTING_TABLE_BASED) Algorithm = new RoutingTableBased();
	if (config.SelectionStrategy() == "RANDOM") Strategy = new SelectionRandom();
	else if (config.SelectionStrategy() == "BUFFER_LEVEL") Strategy = new SelectionBufferLevel();

	// Create and configure tiles
	auto& graph = config.Topology();
	Tiles.resize(graph.size());
	for (int32_t id = 0; id < Tiles.size(); id++)
	{
		char tile_name[32];
		sprintf(tile_name, "Tile[%002d]", id);
		Tiles[id] = new Tile(tile_name, id, graph.size() - 1, graph[id].size(), 
			GlobalParams::stats_warm_up_time, GlobalParams::buffer_depth, 
			*Algorithm, *Strategy, GRTable);
		
		auto& tile = *Tiles[id];

		tile.ConfigureRotuerPower(config.RoutingAlgorithm());

		if (config.TrafficDistribution() == TRAFFIC_TABLE_BASED)
		{
			tile.ProcessingDevice.traffic_table = &GTTable;
			tile.ProcessingDevice.never_transmit = GTTable.occurrencesAsSource(Tiles[id]->ProcessingDevice.local_id) == 0;
		}
		else tile.ProcessingDevice.never_transmit = false;

		tile.clock(clock);
		tile.reset(reset);
	}

	// Connect tiles
	for (int32_t id = 0; id < Tiles.size(); id++)
	{
		auto& tile = *Tiles[id];
		auto& node = graph[id];
		for (int32_t relay = 0; relay < node.size(); relay++)
		{
			if (tile.Relays[relay].Bound()) continue;
			int32_t connected_id = node[relay];

			if (connected_id < 0)
			{
				tile.Relays[relay].SilentBind(BorderRelay);
				tile.Disable(relay);
			}
			else
			{
				auto& connected_tile = *Tiles[connected_id];
				auto& connected_node = graph[connected_id];
				std::vector<int32_t> connected_relays = connected_node.links_to(id);
				for (int32_t rel : connected_relays)
				{
					if (!connected_tile.Relays[rel].Bound())
					{
						tile.Relays[relay].Bind(connected_tile.Relays[rel]);
						break;
					}
				}
			}
			if (!tile.Relays[relay].Bound()) throw std::runtime_error("NoC error: Bad graph.");
		}
	}
		
	// Reset border relay
	BorderRelay.Disable();
}
NoC::~NoC()
{
	for (auto t : Tiles) delete t;
	delete Algorithm;
	delete Strategy;
}

Tile& NoC::SearchNode(int32_t id) const
{
	return *Tiles[id];
}
