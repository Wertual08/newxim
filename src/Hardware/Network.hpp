#pragma once
#include <systemc.h>
#include "Configuration/Configuration.hpp"
#include "Tile.hpp"
#include "Configuration/RoutingTable.hpp"
#include "Configuration/Graph/Graph.hpp"
#include "Configuration/TrafficManagers/TrafficManager.hpp"
#include "Metrics/FlitTracer.hpp"
#include "Routing/RoutingAlgorithm.hpp"
#include "Selection/SelectionStrategy.hpp"



class Network : sc_module
{
private:
	const Configuration& Config;
	std::unique_ptr<RoutingAlgorithm> Algorithm;
	std::unique_ptr<SelectionStrategy> Strategy;
	std::unique_ptr<TrafficManager> Traffic;

	void InitBase();

public:
	std::unique_ptr<FlitTracer> Tracer;
	const SimulationTimer Timer;
	sc_clock clock;
	sc_signal<bool> reset;

	std::vector<Tile> Tiles;

	Network(const Configuration& config, const SimulationTimer& timer, sc_module_name = "NoC");
	~Network();

	friend std::ostream& operator<<(std::ostream& os, const Network& network);
};
