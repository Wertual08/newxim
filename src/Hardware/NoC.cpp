#include "NoC.hpp"
#include "RoutingSelection/RoutingTableBased.hpp"
#include "RoutingSelection/RoutingBypass.hpp"
#include "RoutingSelection/RoutingVirtualSubnetwork.hpp"
#include "RoutingSelection/RoutingFitVirtualSubnetwork.hpp"
#include "RoutingSelection/RoutingSubnetwork.hpp"
#include "RoutingSelection/RoutingFitSubnetwork.hpp"
#include "RoutingSelection/RoutingFixedSubnetwork.hpp"
#include "RoutingSelection/SelectionRandom.hpp"
#include "RoutingSelection/SelectionBufferLevel.hpp"
#include "RoutingSelection/SelectionKeepSpace.hpp"
#include "RoutingSelection/SelectionRandomKeepSpace.hpp"
#include "RoutingSelection/RoutingMeshXY.hpp"
#include "RoutingSelection/RoutingRingSplit.hpp"
#include "RoutingSelection/RoutingVirtualRingSplit.hpp"

#include "Configuration/TrafficManagers/RandomTrafficManager.hpp"
#include "Configuration/TrafficManagers/HotspotTrafficManager.hpp"
#include "Configuration/TrafficManagers/TableTrafficManager.hpp"

#include "Hardware/Router.hpp"



std::unique_ptr<RoutingAlgorithm> GetAlgorithm(const Configuration& config)
{
	if (config.RoutingAlgorithm() == "TABLE_BASED") return std::make_unique<RoutingTableBased>(config.GRTable());
	if (config.RoutingAlgorithm() == "MESH_XY") return std::make_unique<RoutingMeshXY>(config.DimX(), config.DimY(), config.TopologyGraph());
	if (config.RoutingAlgorithm() == "BYPASS") return std::make_unique<RoutingBypass>(config.GRTable(), config.SubGRTable());
	if (config.RoutingAlgorithm() == "SUBNETWORK") return std::make_unique<RoutingSubnetwork>(config.GRTable(), config.SubGRTable());
	if (config.RoutingAlgorithm() == "FIT_SUBNETWORK") return std::make_unique<RoutingFitSubnetwork>(config.GRTable(), config.SubGRTable());
	if (config.RoutingAlgorithm() == "FIXED_SUBNETWORK") return std::make_unique<RoutingFixedSubnetwork>(config.GRTable(), config.SubGRTable());
	if (config.RoutingAlgorithm() == "VIRTUAL_SUBNETWORK") return std::make_unique<RoutingVirtualSubnetwork>(config.GRTable(), config.SubGRTable());
	if (config.RoutingAlgorithm() == "FIT_VIRTUAL_SUBNETWORK") return std::make_unique<RoutingFitVirtualSubnetwork>(config.GRTable(), config.SubGRTable());
	if (config.RoutingAlgorithm() == "RING_SPLIT") return std::make_unique<RoutingRingSplit>(config.NetworkGraph(), config.GRTable());
	if (config.RoutingAlgorithm() == "VIRTUAL_RING_SPLIT") return std::make_unique<RoutingVirtualRingSplit>(config.NetworkGraph(), config.GRTable());
	throw std::runtime_error("Configuration error: Invalid routing algorithm [" + config.RoutingAlgorithm() + "].");
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

	if (Config.ReportFlitTrace()) Tracer = std::make_unique<FlitTracer>(
		Timer, Config.FlitTraceStart(), Config.FlitTraceEnd());
	Algorithm = GetAlgorithm(Config);
	Strategy = GetStrategy(Config);
	Traffic = GetTraffic(Config);

	auto &graph = Config.NetworkGraph();

	// Create and configure tiles
	for (std::int32_t id = 0; id < Tiles.size(); id++)
	{
		std::unique_ptr<Router> RouterDevice = std::make_unique<Router>(Timer, id, graph[id].size());
		for (std::size_t i = 0; i < RouterDevice->Size(); i++)
		{
			auto& relay = (*RouterDevice)[i];
			relay.SetVirtualChannels(Config.VirtualChannels());
			for (std::size_t vc = 0; vc < relay.Size(); vc++)
				relay[vc].Reserve(Config.BufferDepth());
		}

		RouterDevice->SetRoutingAlgorithm(*Algorithm);
		RouterDevice->SetSelectionStrategy(*Strategy);
		if (Tracer) RouterDevice->SetFlitTracer(*Tracer);
		RouterDevice->SetUpdateSequence(Config.UpdateSequence());
		
		std::unique_ptr<Processor> ProcessorDevice = GetProcessor(Timer, id, Config);
		ProcessorDevice->SetTrafficManager(*Traffic);
		if (Tracer) ProcessorDevice->SetFlitTracer(*Tracer);
		ProcessorDevice->relay.SetVirtualChannels(Config.VirtualChannels());
		ProcessorDevice->relay[0].Reserve(Config.BufferDepth());

		auto& tile = Tiles[id];
		tile.SetRouter(RouterDevice);
		tile.SetProcessor(ProcessorDevice);

		tile.clock(clock);
		tile.reset(reset);
	}

	// Connect routers
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

NoC::NoC(const Configuration& config, const SimulationTimer& timer, sc_module_name) :
	Config(config),
	clock("clock", config.ClockPeriodPS(), SC_PS), 
	Timer(timer), 
	Tiles(config.NetworkGraph().size())
{
	InitBase();
}
NoC::~NoC()
{
}

std::ostream& operator<<(std::ostream& os, const NoC& network)
{
	return os;
}
