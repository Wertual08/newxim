#include "Network.hpp"
#include "Hardware/Router.hpp"
#include "Configuration/Factory.hpp"



std::unique_ptr<Processor> GetProcessor(const SimulationTimer& timer, std::int32_t id, const Configuration& config)
{
	return std::make_unique<Processor>(
		timer, 
		id,
		config.MinPacketSize(),
		config.MaxPacketSize());
}

void Network::InitBase()
{
	srand(Config.RndGeneratorSeed());
	Factory factory(Config);
	

	if (Config.ReportFlitTrace()) {
		Tracer = std::make_unique<FlitTracer>(Timer, Config.FlitTraceStart(), Config.FlitTraceEnd());
	}
	Algorithm = factory.MakeAlgorithm();
	Strategy = factory.MakeStrategy();
	Traffic = factory.MakeTraffic();

	auto &graph = Config.NetworkGraph();

	// Create and configure tiles
	for (std::int32_t id = 0; id < Tiles.size(); id++)
	{
		std::unique_ptr<Router> RouterDevice = std::make_unique<Router>(Timer, id, graph[id].size());
		for (std::size_t i = 0; i < RouterDevice->Size(); i++) {
			auto& relay = (*RouterDevice)[i];
			relay.SetVirtualChannels(Config.VirtualChannels());
			for (std::size_t vc = 0; vc < relay.Size(); vc++) {
				relay[vc].Reserve(Config.BufferDepth());
			}
		}

		RouterDevice->SetRoutingAlgorithm(*Algorithm);
		RouterDevice->SetSelectionStrategy(*Strategy);
		if (Tracer) {
			RouterDevice->SetFlitTracer(*Tracer);
		}
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
			for (std::int32_t rel : connected_relays) {
				if (!connected_router[rel].Bound()) {
					router[relay].Bind(connected_router[rel]);
					break;
				}
			}
		}
	}
}

Network::Network(const Configuration& config, const SimulationTimer& timer, sc_module_name) :
	Config(config),
	clock("clock", config.ClockPeriodPS(), SC_PS), 
	Timer(timer), 
	Tiles(config.NetworkGraph().size())
{
	InitBase();
}
Network::~Network()
{
}

std::ostream& operator<<(std::ostream& os, const Network& network)
{
	return os;
}
