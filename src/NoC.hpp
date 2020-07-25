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
	SC_HAS_PROCESS(NoC);
private:
	std::unique_ptr<RoutingAlgorithm> GetAlgorithm(const Configuration& config);
	std::unique_ptr<SelectionStrategy> GetStrategy(const Configuration& config);

	std::unique_ptr<RoutingAlgorithm> Algorithm;
	std::unique_ptr<SelectionStrategy> Strategy;

public:
	sc_clock clock;
	sc_signal<bool> reset;
	std::vector<Tile*> Tiles;

	// Global tables
	RoutingTable GRTable;
	GlobalTrafficTable GTTable;

	NoC(const Configuration& config, sc_module_name = "NoC");
	~NoC();

	void Update();
	friend std::ostream& operator<<(std::ostream& os, const NoC& network);
};
