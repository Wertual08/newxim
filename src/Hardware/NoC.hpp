/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file represents the top-level testbench
 */

#pragma once
#include <systemc.h>
#include "Configuration/Configuration.hpp"
#include "Tile.hpp"
#include "Configuration/RoutingTable.hpp"
#include "Configuration/Graph.hpp"
#include "Configuration/TrafficManagers/TrafficManager.hpp"
#include "Metrics/FlitTracer.hpp"



class NoC : sc_module
{
private:
	const Configuration& Config;
	std::unique_ptr<RoutingAlgorithm> Algorithm;
	std::unique_ptr<SelectionStrategy> Strategy;
	std::unique_ptr<TrafficManager> Traffic;

	void InitBase();
	void InitSubNetwork();

public:
	std::unique_ptr<FlitTracer> Tracer;
	const SimulationTimer Timer;
	sc_clock clock;
	sc_signal<bool> reset;

	std::vector<Tile> Tiles;

	NoC(const Configuration& config, const SimulationTimer& timer, sc_module_name = "NoC");
	~NoC();

	friend std::ostream& operator<<(std::ostream& os, const NoC& network);
};