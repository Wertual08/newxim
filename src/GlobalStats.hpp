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
#include "Configuration.hpp"
#include "NoC.hpp"



class GlobalStats
{
private:
	const Configuration& Config;
	const NoC& Network;
	void updatePowerBreakDown(std::map<std::string, double>& dst, PowerBreakdown* src) const;

	int32_t GetMaxBufferStuckDelay() const;
	double GetAverageBufferLoad(int32_t channel) const;
	double GetAverageBufferLoad() const;
	int32_t getLastReceivedFlitTime() const;
	// Returns the aggregated average delay (cycles)
	double getAverageDelay() const;
	// Returns the aggragated average delay (cycles) for communication src_id->dst_id
	double getAverageDelay(int src_id, int dst_id) const;
	// Returns the max delay
	double getMaxDelay() const;
	// Returns the max delay (cycles) experimented by destination
	// node_id. Returns -1 if node_id is not destination of any
	// communication
	double getMaxDelay(int32_t node_id) const;
	// Returns the max delay (cycles) for communication src_id->dst_id
	double getMaxDelay(int src_id, int dst_id) const;
	// Returns the aggregated average throughput (flits/cycles)
	double getAggregatedProduction() const;
	double getAggregatedThroughput() const;
	double getAggregatedAcceptance() const;
	// Returns the average throughput per IP (flit/cycles/IP)
	double getThroughput() const;
	// Returns the average throughput considering only a active IP (flit/cycles/IP)
	double getActiveThroughput() const;
	// Returns the aggregated average throughput (flits/cycles) for
	// communication src_id->dst_id
	double getAverageThroughput(int src_id, int dst_id);
	// Returns the total number of received packets
	unsigned int getReceivedPackets() const;
	// Returns the total number of received flits
	unsigned int getReceivedFlits() const;
	unsigned int getAcceptedFlits() const;
	// Returns the total dyamic power
	double getDynamicPower() const;
	// Returns the total static power
	double getStaticPower() const;
	// Returns the total power
	double getTotalPower() const { return getDynamicPower() + getStaticPower(); }
	double getReceivedIdealFlitRatio() const;

	int32_t GetTotalFlitsGenerated() const;

	void ShowBuffers(std::ostream& out) const;

public:
	GlobalStats(const NoC& network, const Configuration& config);

	void showPowerBreakDown(std::ostream& out);

	// Shows global statistics
	friend std::ostream& operator<<(std::ostream& out, const GlobalStats& gs);
};
