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
#include "RoutingSelection/RoutingTableBased.h"
#include "RoutingSelection/SelectionRandom.h"
#include "RoutingSelection/SelectionBufferLevel.h"
#include "RoutingSelection/SelectionKeepSpace.h"
#include "RoutingSelection/RoutingMeshXY.h"
#include "RoutingSelection/RoutingTorusXY.h"
#include "RoutingSelection/RoutingChannelIndexStep.h"
#include "RoutingSelection/RoutingTests.h"



NoC::NoC(const Configuration& config, sc_module_name) :
	clock("clock", GlobalParams::clock_period_ps, SC_PS), 
	GRTable(config.GRTable())
{
	srand(config.RndGeneratorSeed());
	//std::cout << config.Topology() << '\n';
	//std::cout << config.GRTable() << '\n';

	// Check for traffic table availability
	if (config.TrafficDistribution() == TRAFFIC_TABLE_BASED)
		GTTable.load(config.TrafficTableFilename().c_str());

	if (config.RoutingAlgorithm() == ROUTING_TABLE_BASED) Algorithm = new RoutingTableBased();
	else if (config.RoutingAlgorithm() == "MESH_XY") Algorithm = new RoutingMeshXY(config.DimX(), config.DimY(), config.Topology());
	else if (config.RoutingAlgorithm() == "TORUS_XY") Algorithm = new RoutingTorusXY(config.DimX(), config.DimY(), config.Topology());
	else Algorithm = FindTestRouting(config.RoutingAlgorithm(), config);
	if (config.SelectionStrategy() == "RANDOM") Strategy = new SelectionRandom();
	else if (config.SelectionStrategy() == "BUFFER_LEVEL") Strategy = new SelectionBufferLevel();
	else if (config.SelectionStrategy() == "KEEP_SPACE") Strategy = new SelectionKeepSpace();

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
	}

	SC_METHOD(Update);
	sensitive << reset << clock.value_changed_event();
}
NoC::~NoC()
{
	for (auto t : Tiles) delete t;
	delete Algorithm;
	delete Strategy;
}

void NoC::Update()
{
}

std::ostream& operator<<(std::ostream& os, const NoC& network)
{
	return os;
}
