/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the power model
 */

#pragma once
#include <cassert>
#include <map>
#include "DataStructs.hpp"
#include "SimulationTimer.hpp"
#include "Configuration.hpp"

#include "yaml-cpp/yaml.h"



class Power 
{
private:
	const SimulationTimer Timer;
	double W2J(double watt);

public:
	Power(const SimulationTimer& timer);

	void configureRouter(
		const Configuration::Power& config,
		int link_width,
		int buffer_depth,
		int buffer_item_size,
		std::string routing_function,
		std::string selection_function);

	void bufferRouterPush();
	void bufferRouterPop();
	void bufferRouterFront();

	void routing();
	void selection();
	void crossBar();
	void r2hLink();
	void r2rLink();
	void networkInterface();

	void leakageBufferRouter();
	void leakageBufferToTile();
	void leakageBufferFromTile();
	void leakageAntennaBuffer();
	void leakageLinkRouter2Router();
	void leakageRouter();
	void leakageTransceiverRx();
	void leakageTransceiverTx();
	void biasingRx();
	void biasingTx();

	double getDynamicPower();
	double getStaticPower();

	double getTotalPower() {
		return (getDynamicPower() + getStaticPower());
	}


	void printBreakDown(std::ostream& out);


	PowerBreakdown* getDynamicPowerBreakDown() { return &power_dynamic; }
	PowerBreakdown* getStaticPowerBreakDown() { return &power_static; }

	void rxSleep(int cycles);
	bool isSleeping();

private:

	double total_power_s;

	double buffer_router_push_pwr_d;
	double buffer_router_pop_pwr_d;
	double buffer_router_front_pwr_d;
	double buffer_router_pwr_s;

	double buffer_to_tile_push_pwr_d;
	double buffer_to_tile_pop_pwr_d;
	double buffer_to_tile_front_pwr_d;
	double buffer_to_tile_pwr_s;

	double buffer_from_tile_push_pwr_d;
	double buffer_from_tile_pop_pwr_d;
	double buffer_from_tile_front_pwr_d;
	double buffer_from_tile_pwr_s;

	double default_tx_energy;

	double routing_pwr_d;
	double routing_pwr_s;

	double selection_pwr_d;
	double selection_pwr_s;

	double crossbar_pwr_d;
	double crossbar_pwr_s;

	double link_r2r_pwr_d;
	double link_r2r_pwr_s;

	double ni_pwr_d;
	double ni_pwr_s;

	std::map<std::pair<int, int>, double>  attenuation_map;
	double attenuation2power(double);

	PowerBreakdown power_dynamic;
	PowerBreakdown power_static;

	void initPowerBreakdownEntry(PowerBreakdownEntry* pbe, std::string label);
	void initPowerBreakdown();

	int sleep_end_cycle;
};
