#include "Factory.hpp"
#include <memory>

#include "Configuration/Configuration.hpp"

#include "Routing/RoutingTableBased.hpp"
#include "Routing/RoutingBypass.hpp"
#include "Routing/RoutingVirtualSubnetwork.hpp"
#include "Routing/RoutingFitVirtualSubnetwork.hpp"
#include "Routing/RoutingSubnetwork.hpp"
#include "Routing/RoutingFitSubnetwork.hpp"
#include "Routing/RoutingFixedSubnetwork.hpp"
#include "Routing/RoutingMeshXY.hpp"
#include "Routing/RoutingMeshWestFirst.hpp"
#include "Routing/RoutingMeshO1TURN.hpp"
#include "Routing/RoutingMeshXYYX.hpp"
#include "Routing/RoutingMeshNegativeFirst.hpp"
#include "Routing/RoutingMeshNorthLast.hpp"
#include "Routing/RoutingMeshOddEven.hpp"
#include "Routing/RoutingTorusCLUE.hpp"

#include "Selection/SelectionRandom.hpp"
#include "Selection/SelectionBufferLevel.hpp"
#include "Selection/SelectionKeepSpace.hpp"
#include "Selection/SelectionRandomKeepSpace.hpp"
#include "Selection/SelectionRingSplit.hpp"
#include "Selection/SelectionVirtualRingSplit.hpp"

#include "Configuration/TrafficManagers/RandomTrafficManager.hpp"
#include "Configuration/TrafficManagers/HotspotTrafficManager.hpp"
#include "Configuration/TrafficManagers/TableTrafficManager.hpp"



std::unique_ptr<RoutingAlgorithm> Factory::MakeAlgorithm() const
{
	if (config.RoutingAlgorithm() == "TABLE_BASED") return std::make_unique<RoutingTableBased>(config.GRTable());

	if (config.RoutingAlgorithm() == "MESH_XY") return std::make_unique<RoutingMeshXY>(config.DimX(), config.DimY(), config.TopologyGraph());
	if (config.RoutingAlgorithm() == "MESH_WEST_FIRST") return std::make_unique<RoutingMeshWestFirst>(config.DimX(), config.DimY(), config.TopologyGraph());
	if (config.RoutingAlgorithm() == "MESH_O1TURN") return std::make_unique<RoutingMeshO1TURN>(config.DimX(), config.DimY(), config.TopologyGraph());
	if (config.RoutingAlgorithm() == "MESH_XY_YX") return std::make_unique<RoutingMeshXYYX>(config.DimX(), config.DimY(), config.TopologyGraph());
	if (config.RoutingAlgorithm() == "MESH_NEGATIVE_FIRST") return std::make_unique<RoutingMeshNegativeFirst>(config.DimX(), config.DimY(), config.TopologyGraph());
	if (config.RoutingAlgorithm() == "MESH_NORTH_LAST") return std::make_unique<RoutingMeshNorthLast>(config.DimX(), config.DimY(), config.TopologyGraph());
	if (config.RoutingAlgorithm() == "MESH_ODD_EVEN") return std::make_unique<RoutingMeshOddEven>(config.DimX(), config.DimY(), config.TopologyGraph());

	if (config.RoutingAlgorithm() == "TORUS_CLUE") return std::make_unique<RoutingTorusCLUE>(config.DimX(), config.DimY(), config.TopologyGraph());

	if (config.RoutingAlgorithm() == "BYPASS") return std::make_unique<RoutingBypass>(config.GRTable(), config.SubGRTable());
	if (config.RoutingAlgorithm() == "SUBNETWORK") return std::make_unique<RoutingSubnetwork>(config.GRTable(), config.SubGRTable());
	if (config.RoutingAlgorithm() == "FIT_SUBNETWORK") return std::make_unique<RoutingFitSubnetwork>(config.GRTable(), config.SubGRTable());
	if (config.RoutingAlgorithm() == "FIXED_SUBNETWORK") return std::make_unique<RoutingFixedSubnetwork>(config.GRTable(), config.SubGRTable());
	if (config.RoutingAlgorithm() == "VIRTUAL_SUBNETWORK") return std::make_unique<RoutingVirtualSubnetwork>(config.GRTable(), config.SubGRTable());
	if (config.RoutingAlgorithm() == "FIT_VIRTUAL_SUBNETWORK") return std::make_unique<RoutingFitVirtualSubnetwork>(config.GRTable(), config.SubGRTable());
	throw std::runtime_error("Configuration error: Invalid routing algorithm [" + config.RoutingAlgorithm() + "].");
}

std::unique_ptr<SelectionStrategy> Factory::MakeStrategy() const
{
	if (config.SelectionStrategy() == "RANDOM") return std::make_unique<SelectionRandom>();
	if (config.SelectionStrategy() == "BUFFER_LEVEL") return std::make_unique<SelectionBufferLevel>();
	if (config.SelectionStrategy() == "KEEP_SPACE") return std::make_unique<SelectionKeepSpace>();
	if (config.SelectionStrategy() == "RANDOM_KEEP_SPACE") return std::make_unique<SelectionRandomKeepSpace>();
	if (config.SelectionStrategy() == "RING_SPLIT") return std::make_unique<SelectionRingSplit>();
	if (config.SelectionStrategy() == "VIRTUAL_RING_SPLIT") return std::make_unique<SelectionVirtualRingSplit>(config.TopologyGraph());
	throw std::runtime_error("Configuration error: Invalid selection strategy [" + config.SelectionStrategy() + "].");
}

std::unique_ptr<TrafficManager> Factory::MakeTraffic() const
{
	if (config.TrafficDistribution() == "TRAFFIC_RANDOM") return std::make_unique<RandomTrafficManager>(
		config.RndGeneratorSeed(), config.TopologyGraph().size(), config.PacketInjectionRate());
	if (config.TrafficDistribution() == "TRAFFIC_HOTSPOT") return std::make_unique<HotspotTrafficManager>(
		config.RndGeneratorSeed(), config.TopologyGraph().size(), config.PacketInjectionRate(), config.Hotspots());
	if (config.TrafficDistribution() == "TRAFFIC_TABLE_BASED") return std::make_unique<TableTrafficManager>(
		config.RndGeneratorSeed(), config.TopologyGraph().size(), config.TrafficTableFilename(), config.PacketInjectionRate(), config.SimulationTime());
	throw std::runtime_error("Configuration error: Invalid traffic distribution [" + config.TrafficDistribution() + "].");
}
