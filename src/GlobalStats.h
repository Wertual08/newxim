/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the global statistics
 */

#pragma once
#include <vector>
#include <map>
#include "NoC.h"
#include "Tile.h"



class GlobalStats
{
private:
	const NoC& Network;
	void updatePowerBreakDown(std::map<std::string, double>& dst, PowerBreakdown* src);

public:
	GlobalStats(const NoC& _noc);

	// Returns the aggregated average delay (cycles)
	double getAverageDelay();

	// Returns the aggragated average delay (cycles) for communication src_id->dst_id
	double getAverageDelay(int src_id, int dst_id);

	// Returns the max delay
	double getMaxDelay();

	// Returns the max delay (cycles) experimented by destination
	// node_id. Returns -1 if node_id is not destination of any
	// communication
	double getMaxDelay(int32_t node_id);

	// Returns the max delay (cycles) for communication src_id->dst_id
	double getMaxDelay(int src_id, int dst_id);

	// Returns the aggregated average throughput (flits/cycles)
	double getAggregatedThroughput();

	// Returns the average throughput per IP (flit/cycles/IP)
	double getThroughput();

	// Returns the average throughput considering only a active IP (flit/cycles/IP)
	double getActiveThroughput();

	// Returns the aggregated average throughput (flits/cycles) for
	// communication src_id->dst_id
	double getAverageThroughput(int src_id, int dst_id);

	// Returns the total number of received packets
	unsigned int getReceivedPackets();

	// Returns the total number of received flits
	unsigned int getReceivedFlits();

	// Returns the total dyamic power
	double getDynamicPower();
	// Returns the total static power
	double getStaticPower();

	// Returns the total power
	double getTotalPower() { return getDynamicPower() + getStaticPower(); }

	// Shows global statistics
	void showStats(std::ostream& out = std::cout, bool detailed = false);

	void showBufferStats(std::ostream& out);

	void showPowerBreakDown(std::ostream& out);

	void showPowerManagerStats(std::ostream& out);

	double getReceivedIdealFlitRatio();
};
