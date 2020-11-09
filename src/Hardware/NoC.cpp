#include "NoC.hpp"
#include "RoutingSelection/RoutingTableBased.hpp"
#include "RoutingSelection/RoutingFitSubnetwork.hpp"
#include "RoutingSelection/SelectionRandom.hpp"
#include "RoutingSelection/SelectionBufferLevel.hpp"
#include "RoutingSelection/SelectionKeepSpace.hpp"
#include "RoutingSelection/SelectionRandomKeepSpace.hpp"
#include "RoutingSelection/RoutingMeshXY.hpp"
#include "RoutingSelection/RoutingTorusXY.hpp"
#include "RoutingSelection/RoutingTests.hpp"

#include "Configuration/TrafficManagers/RandomTrafficManager.hpp"
#include "Configuration/TrafficManagers/HotspotTrafficManager.hpp"
#include "Configuration/TrafficManagers/TableTrafficManager.hpp"

#include "Hardware/Routers/WormholeRouter.hpp"
#include "Hardware/Routers/PerFlitRouter.hpp"
#include "Hardware/Routers/PerFlitSubnetworkRouter.hpp"
#include "Hardware/Routers/WormholeSubnetworkRouter.hpp"
#include "Hardware/Routers/WormholeFixedSubnetworkRouter.hpp"
#include "Hardware/Routers/WormholeFitSubnetworkRouter.hpp"



std::unique_ptr<RoutingAlgorithm> GetAlgorithm(const Configuration& config)
{
	if (config.RoutingAlgorithm() == "TABLE_BASED") return std::make_unique<RoutingTableBased>(config.GRTable());
	if (config.RoutingAlgorithm() == "FIT_SUBNETWORK") return std::make_unique<RoutingFitSubnetwork>(config.GRTable(), config.VirtualSubGRTable());
	if (config.RoutingAlgorithm() == "MESH_XY") return std::make_unique<RoutingMeshXY>(config.DimX(), config.DimY(), config.TopologyGraph());
	if (config.RoutingAlgorithm() == "TORUS_XY") return std::make_unique<RoutingTorusXY>(config.DimX(), config.DimY(), config.TopologyGraph());
	return FindTestRouting(config.RoutingAlgorithm(), config, config.GRTable());
}
std::unique_ptr<SelectionStrategy> GetStrategy(const Configuration& config)
{
	if (config.SelectionStrategy() == "RANDOM") return std::make_unique<SelectionRandom>();
	if (config.SelectionStrategy() == "BUFFER_LEVEL") return std::make_unique<SelectionBufferLevel>();
	if (config.SelectionStrategy() == "KEEP_SPACE") return std::make_unique<SelectionKeepSpace>();
	if (config.SelectionStrategy() == "RANDOM_KEEP_SPACE") return std::make_unique<SelectionRandomKeepSpace>();
	throw std::runtime_error("Configuration error: Invalid selection strategy [" + config.SelectionStrategy() + "].");
}
std::unique_ptr<TrafficManager> GetTraffic(const Configuration& config)
{
	if (config.TrafficDistribution() == "TRAFFIC_RANDOM") return std::make_unique<RandomTrafficManager>(
		config.RndGeneratorSeed(), config.TopologyGraph().size(), config.PacketInjectionRate(), config.ProbabilityOfRetransmission());
	if (config.TrafficDistribution() == "TRAFFIC_HOTSPOT") return std::make_unique<HotspotTrafficManager>(
		config.RndGeneratorSeed(), config.TopologyGraph().size(), config.PacketInjectionRate(), config.ProbabilityOfRetransmission(), config.Hotspots());
	if (config.TrafficDistribution() == "TRAFFIC_TABLE_BASED") return std::make_unique<TableTrafficManager>(
		config.RndGeneratorSeed(), config.TopologyGraph().size(), config.TrafficTableFilename(), config.PacketInjectionRate(), config.ProbabilityOfRetransmission());
	throw std::runtime_error("Configuration error: Invalid traffic distribution [" + config.TrafficDistribution() + "].");
}
std::unique_ptr<Router> GetRouter(const SimulationTimer& timer, std::int32_t id, const Configuration& config)
{
	std::size_t relays_count = config.TopologyGraph()[id].size();
	if (config.RouterType() == "WORMHOLE") return std::make_unique<WormholeRouter>(timer, id, relays_count);
	if (config.RouterType() == "PER_FLIT") return std::make_unique<PerFlitRouter>(timer, id, relays_count);
	if (config.RouterType() == "PER_FLIT_SUBNETWORK") return std::make_unique<PerFlitSubnetworkRouter>(timer, id, relays_count + config.TopologySubGraph()[id].size());
	if (config.RouterType() == "WORMHOLE_SUBNETWORK") return std::make_unique<WormholeSubnetworkRouter>(timer, id, relays_count + config.TopologySubGraph()[id].size());
	if (config.RouterType() == "WORMHOLE_FIXED_SUBNETWORK") return std::make_unique<WormholeFixedSubnetworkRouter>(timer, id, relays_count + config.TopologySubGraph()[id].size());
	if (config.RouterType() == "WORMHOLE_FIT_SUBNETWORK") return std::make_unique<WormholeFitSubnetworkRouter>(timer, id, relays_count + config.TopologySubGraph()[id].size());
	throw std::runtime_error("Configuration error: Invalid router type [" + config.RouterType() + "].");
}
std::unique_ptr<Processor> GetProcessor(const SimulationTimer& timer, std::int32_t id, const Configuration& config)
{
	return std::make_unique<Processor>(
		timer, 
		id,
		config.MinPacketSize(),
		config.MaxPacketSize());
}

void NoC::InitBase()
{
	srand(Config.RndGeneratorSeed());

	Algorithm = GetAlgorithm(Config);
	Strategy = GetStrategy(Config);
	Traffic = GetTraffic(Config);

	// Create and configure tiles
	for (std::int32_t id = 0; id < Tiles.size(); id++)
	{
		std::unique_ptr<Router> RouterDevice = GetRouter(Timer, id, Config);
		for (std::size_t i = 0; i < RouterDevice->Size(); i++)
		{
			auto& relay = (*RouterDevice)[i];
			relay.SetVirtualChannels(Config.VirtualChannels());
			for (std::size_t vc = 0; vc < relay.Size(); vc++)
				relay[vc].Reserve(Config.BufferDepth());
		}

		RouterDevice->SetRoutingAlgorithm(*Algorithm);
		RouterDevice->SetSelectionStrategy(*Strategy);
		RouterDevice->power.configureRouter(Config.PowerConfiguration(), Config.FlitSize(), Config.BufferDepth(), Config.FlitSize(), Config.RoutingAlgorithm(), "default");

		std::unique_ptr<Processor> ProcessorDevice = GetProcessor(Timer, id, Config);
		ProcessorDevice->SetTrafficManager(*Traffic);
		ProcessorDevice->relay.SetVirtualChannels(Config.VirtualChannels());
		ProcessorDevice->relay[0].Reserve(Config.BufferDepth());

		auto& tile = Tiles[id];
		tile.SetRouter(RouterDevice);
		tile.SetProcessor(ProcessorDevice);

		tile.clock(clock);
		tile.reset(reset);
	}

	// Connect routers
	auto& graph = Config.TopologyGraph();
	for (std::int32_t id = 0; id < Tiles.size(); id++)
	{
		auto& tile = Tiles[id];
		auto& router = *tile.RouterDevice;
		auto& processor = *tile.ProcessorDevice;
		auto& node = graph[id];

		router.LocalRelay.Bind(processor.relay);

		for (std::int32_t relay = 0; relay < node.size(); relay++)
		{
			if (router[relay].Bound()) continue;
			std::int32_t connected_id = node[relay];

			auto& connected_tile = Tiles[connected_id];
			auto& connected_router = *connected_tile.RouterDevice;
			auto& connected_node = graph[connected_id];
			std::vector<std::int32_t> connected_relays = connected_node.links_to(id);
			for (std::int32_t rel : connected_relays)
			{
				if (!connected_router[rel].Bound())
				{
					router[relay].Bind(connected_router[rel]);
					break;
				}
			}
		}
	}
}
void NoC::InitSubNetwork()
{
	auto& graph = Config.TopologyGraph();
	auto& sub_graph = Config.TopologySubGraph();
	auto& sub_table = Config.SubGRTable();

	//std::cout << "Subtree weiner index: " << sub_graph.weiner_index() << '\n';

	for (std::int32_t id = 0; id < Tiles.size(); id++)
	{
		auto& tile = Tiles[id];
		auto& router = *dynamic_cast<SubnetworkRouter*>(tile.RouterDevice.get());
		auto& node = graph[id];
		auto& sub_node = sub_graph[id];

		auto sub_table_node = sub_table[id];
		for (auto& vec : sub_table_node)
			for (auto& val : vec) val += node.size();
		router.SetupSubnetworkTable(sub_table_node);

		for (std::int32_t i = 0; i < sub_node.size(); i++)
		{
			std::int32_t relay = node.size() + i;
			if (router[relay].Bound()) continue;
			std::int32_t connected_id = sub_node[i];

			auto& connected_tile = Tiles[connected_id];
			auto& connected_router = *connected_tile.RouterDevice;
			auto& connected_node = graph[connected_id];
			auto& connected_sub_node = sub_graph[connected_id];

			std::vector<std::int32_t> connected_relays = connected_sub_node.links_to(id);
			for (std::int32_t rel : connected_relays)
			{
				rel += connected_node.size();
				if (!connected_router[rel].Bound())
				{
					router[relay].Bind(connected_router[rel]);
					break;
				}
			}
		}
	}
}

NoC::NoC(const Configuration& config, const SimulationTimer& timer, sc_module_name) :
	Config(config),
	clock("clock", config.ClockPeriodPS(), SC_PS), 
	Timer(timer), 
	Tiles(config.TopologyGraph().size())
{
	InitBase();

	if (config.RouterType() == "PER_FLIT_SUBNETWORK" ||
		config.RouterType() == "WORMHOLE_SUBNETWORK" ||
		config.RouterType() == "WORMHOLE_FIXED_SUBNETWORK" ||
		config.RouterType() == "WORMHOLE_FIT_SUBNETWORK")
	{
		InitSubNetwork();
	}
}
NoC::~NoC()
{
}

std::ostream& operator<<(std::ostream& os, const NoC& network)
{
	return os;
}
