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
#include "RoutingSelection/RoutingTests.h"

#include "WormholeRouter.h"
#include "PerFlitRouter.h"



std::unique_ptr<RoutingAlgorithm> NoC::GetAlgorithm(const Configuration& config)
{
	if (config.RoutingAlgorithm() == ROUTING_TABLE_BASED) return std::make_unique<RoutingTableBased>(GRTable);
	else if (config.RoutingAlgorithm() == "MESH_XY") return std::make_unique<RoutingMeshXY>(config.DimX(), config.DimY(), config.Topology());
	else if (config.RoutingAlgorithm() == "TORUS_XY") return std::make_unique<RoutingTorusXY>(config.DimX(), config.DimY(), config.Topology());
	else FindTestRouting(config.RoutingAlgorithm(), config, GRTable);
}
std::unique_ptr<SelectionStrategy> NoC::GetStrategy(const Configuration& config)
{
	if (config.SelectionStrategy() == "RANDOM") return std::make_unique<SelectionRandom>();
	else if (config.SelectionStrategy() == "BUFFER_LEVEL") return std::make_unique<SelectionBufferLevel>();
	else if (config.SelectionStrategy() == "KEEP_SPACE") return std::make_unique<SelectionKeepSpace>();
	else return nullptr;
}

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

	Algorithm = GetAlgorithm(config);
	Strategy = GetStrategy(config);

	// Create and configure tiles
	auto& graph = config.Topology();
	Tiles.resize(graph.size());
	for (int32_t id = 0; id < Tiles.size(); id++)
	{
		char name[32];


		sprintf(name, "Router[%002d]", id);
		std::unique_ptr<Router> RouterDevice;

		if (config.RouterType() == "WORMHOLE") RouterDevice = std::make_unique<WormholeRouter>(
			name, id, graph[id].size(), GlobalParams::buffer_depth, *Algorithm, *Strategy);
		else if (config.RouterType() == "PER_FLIT") RouterDevice = std::make_unique<PerFlitRouter>(
			name, id, graph[id].size(), GlobalParams::buffer_depth, *Algorithm, *Strategy);

		RouterDevice->stats.SetWarmUpTime(GlobalParams::stats_warm_up_time);


;
		sprintf(name, "Processor[%002d]", id);
		std::unique_ptr<Processor> ProcessorDevice;
		ProcessorDevice = std::make_unique<Processor>(name, graph.size() - 1, config.Traffic());
		ProcessorDevice->local_id = id;
		if (config.TrafficDistribution() == TRAFFIC_TABLE_BASED)
		{
			ProcessorDevice->traffic_table = &GTTable;
			ProcessorDevice->never_transmit = GTTable.occurrencesAsSource(ProcessorDevice->local_id) == 0;
		}
		else ProcessorDevice->never_transmit = false;


		sprintf(name, "Tile[%002d]", id);
		Tiles[id] = new Tile(name, graph[id].size());
		auto& tile = *Tiles[id];

		tile.SetRouter(RouterDevice);
		tile.SetProcessor(ProcessorDevice);

		tile.ConfigureRotuerPower(config.RoutingAlgorithm());

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
}

void NoC::Update()
{
}

std::ostream& operator<<(std::ostream& os, const NoC& network)
{
	return os;
}
