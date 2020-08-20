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
#include "Configuration.hpp"
#include "Tile.hpp"
#include "RoutingTable.hpp"
#include "GlobalTrafficTable.hpp"
#include "Graph.hpp"
#include "TrafficManager.hpp"



class NoC : sc_module
{
private:
	const Configuration& Config;
	const SimulationTimer Timer;
	std::unique_ptr<RoutingAlgorithm> Algorithm;
	std::unique_ptr<SelectionStrategy> Strategy;
	std::unique_ptr<TrafficManager> Traffic;

	void InitBase();

public:
	sc_clock clock;
	sc_signal<bool> reset;

	std::vector<Tile> Tiles;

	NoC(const Configuration& config, const SimulationTimer& timer, sc_module_name = "NoC");
	~NoC();

	friend std::ostream& operator<<(std::ostream& os, const NoC& network);
};
