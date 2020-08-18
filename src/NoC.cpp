/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the Network-on-Chip
 */

#include "NoC.hpp"
#include "RoutingSelection/RoutingTableBased.hpp"
#include "RoutingSelection/SelectionRandom.hpp"
#include "RoutingSelection/SelectionBufferLevel.hpp"
#include "RoutingSelection/SelectionKeepSpace.hpp"
#include "RoutingSelection/SelectionRandomKeepSpace.hpp"
#include "RoutingSelection/RoutingMeshXY.hpp"
#include "RoutingSelection/RoutingTorusXY.hpp"
#include "RoutingSelection/RoutingTests.hpp"

#include "WormholeRouter.hpp"
#include "PerFlitRouter.hpp"



std::unique_ptr<RoutingAlgorithm> NoC::GetAlgorithm(const Configuration& config)
{
	if (config.RoutingAlgorithm() == "TABLE_BASED") return std::make_unique<RoutingTableBased>(GRTable);
	else if (config.RoutingAlgorithm() == "MESH_XY") return std::make_unique<RoutingMeshXY>(config.DimX(), config.DimY(), config.Topology());
	else if (config.RoutingAlgorithm() == "TORUS_XY") return std::make_unique<RoutingTorusXY>(config.DimX(), config.DimY(), config.Topology());
	else return FindTestRouting(config.RoutingAlgorithm(), config, GRTable);
}
std::unique_ptr<SelectionStrategy> NoC::GetStrategy(const Configuration& config)
{
	if (config.SelectionStrategy() == "RANDOM") return std::make_unique<SelectionRandom>();
	else if (config.SelectionStrategy() == "BUFFER_LEVEL") return std::make_unique<SelectionBufferLevel>();
	else if (config.SelectionStrategy() == "KEEP_SPACE") return std::make_unique<SelectionKeepSpace>();
	else if (config.SelectionStrategy() == "RANDOM_KEEP_SPACE") return std::make_unique<SelectionRandomKeepSpace>();
	else return nullptr;
}

NoC::NoC(const Configuration& config, const SimulationTimer& timer, sc_module_name) :
	clock("clock", config.ClockPeriodPS(), SC_PS), 
	Timer(timer), GRTable(config.GRTable()),
	GTTable(config.ResetTime(), config.SimulationTime(), config.PacketInjectionRate())
{
	srand(config.RndGeneratorSeed());
	//std::cout << config.Topology() << '\n';
	//std::cout << config.GRTable() << '\n';

	// Check for traffic table availability
	if (config.TrafficDistribution() == "TRAFFIC_TABLE_BASED")
		GTTable.load(config.TrafficTableFilename().c_str());

	Algorithm = GetAlgorithm(config);
	Strategy = GetStrategy(config);

	// Create and configure tiles
	auto& graph = config.Topology();
	Tiles.resize(graph.size());
	for (std::int32_t id = 0; id < Tiles.size(); id++)
	{
		std::unique_ptr<Router> RouterDevice;

		if (config.RouterType() == "WORMHOLE") RouterDevice = std::make_unique<WormholeRouter>(
			Timer, id, graph[id].size(), config.BufferDepth());
		else if (config.RouterType() == "PER_FLIT") RouterDevice = std::make_unique<PerFlitRouter>(
			Timer, id, graph[id].size(), config.BufferDepth());

		RouterDevice->SetRoutingAlgorithm(*Algorithm);
		RouterDevice->SetSelectionStrategy(*Strategy);
;
		std::unique_ptr<Processor> ProcessorDevice = std::make_unique<Processor>(
			Timer, id, config.TrafficDistribution() == "TRAFFIC_TABLE_BASED",
			config.PacketInjectionRate(), config.ProbabilityOfRetransmission(), config.MinPacketSize(), 
			config.MaxPacketSize(), config.Topology().size() - 1);

		ProcessorDevice->SetTrafficManager(config.Traffic());

		if (config.TrafficDistribution() == "TRAFFIC_TABLE_BASED")
		{
			ProcessorDevice->traffic_table = &GTTable;
			ProcessorDevice->never_transmit = GTTable.occurrencesAsSource(ProcessorDevice->local_id) == 0;
		}
		else ProcessorDevice->never_transmit = false;

		char name[32];
		sprintf(name, "Tile[%002d]", id);
		Tiles[id] = new Tile(name);
		auto& tile = *Tiles[id];

		tile.SetRouter(RouterDevice);
		tile.SetProcessor(ProcessorDevice);

		tile.ConfigureRotuerPower(config.PowerConfiguration(), config.FlitSize(), config.BufferDepth(), config.RoutingAlgorithm());

		tile.clock(clock);
		tile.reset(reset);
	}

	// Connect tiles
	for (std::int32_t id = 0; id < Tiles.size(); id++)
	{
		auto& tile = *Tiles[id];
		auto& router = *tile.RouterDevice;
		auto& processor = *tile.ProcessorDevice;
		auto& node = graph[id];

		router.LocalRelay.Bind(processor.relay);

		for (std::int32_t relay = 0; relay < node.size(); relay++)
		{
			if (router.Relays[relay].Bound()) continue;
			std::int32_t connected_id = node[relay];

			auto& connected_tile = *Tiles[connected_id];
			auto& connected_router = *connected_tile.RouterDevice;
			auto& connected_node = graph[connected_id];
			std::vector<std::int32_t> connected_relays = connected_node.links_to(id);
			for (std::int32_t rel : connected_relays)
			{
				if (!connected_router.Relays[rel].Bound())
				{
					router.Relays[relay].Bind(connected_router.Relays[rel]);
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
