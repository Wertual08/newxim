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
#include "Configuration.h"
#include "Tile.h"
#include "RoutingTable.h"
#include "GlobalTrafficTable.h"
#include "Graph.h"



class NoC
{
private:
	Tile::Relay BorderRelay;

public:
	sc_clock clock;
	sc_signal<bool> reset;

	RoutingAlgorithm* Algorithm;
	SelectionStrategy* Strategy;
	std::vector<Tile*> Tiles;

	// Global tables
	RoutingTable GRTable;
	GlobalTrafficTable GTTable;

	NoC(const Configuration& config);
	~NoC();

	// Support methods
	Tile& SearchNode(int32_t id) const;
};
