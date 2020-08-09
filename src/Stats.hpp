#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include "DataStructs.hpp"
#include "Power.hpp"
#include "SimulationTimer.hpp"



struct CommHistory 
{
	int32_t src_id;

	uint32_t total_received_packets; 
	double total_packets_delay; 
	double packets_max_delay; 

	uint32_t total_received_flits;
	double last_received_flit_time;
};

class Stats 
{
private:
	const SimulationTimer Timer;
	int32_t id;
	std::vector<CommHistory> chist;
	double last_received_flit_time;
	int32_t total_flits_accepted;

	std::vector<std::pair<double, double>> AVGBufferLoad;
	std::vector<double> LastBufferPopOrEmptyTime;

	int searchCommHistory(int src_id);

public:
	Stats(const SimulationTimer& timer, int32_t node_id, int32_t buffers);

	void UpdateBufferPopOrEmptyTime(int32_t buffer);
	double GetBufferPopOrEmptyTime(int32_t buffer) const;
	double GetMinBufferPopOrEmptyTime() const;

	void AcceptFlit();
	int32_t GetAcceptedFlits() const;

	void UpdateBufferLoad(int32_t buffer, double load);
	double GetAVGBufferLoad(int32_t channel, int32_t channels_count);

	double getLastReceivedFlitTime() const;

	// Access point for stats update
	void receivedFlit(const Flit& flit);

	// Returns the average delay (cycles) for the current node as
	// regards to the communication whose source is src_id
	double getAverageDelay(int src_id);

	// Returns the average delay (cycles) for the current node
	double getAverageDelay();

	// Returns the max delay for the current node as regards the
	// communication whose source node is src_id
	double getMaxDelay(int src_id);

	// Returns the max delay (cycles) for the current node
	double getMaxDelay();

	// Returns the average throughput (flits/cycle) for the current node
	// and for the communication whose source is src_id
	double getAverageThroughput(int src_id);

	// Returns the average throughput (flits/cycle) for the current node
	double getAverageThroughput();

	// Returns the number of received packets from current node
	unsigned int getReceivedPackets();

	// Returns the number of received flits from current node
	unsigned int getReceivedFlits();

	// Returns the number of communications whose destination is the
	// current node
	unsigned int getTotalCommunications();

	// Returns the energy consumed for communication src_id-->dst_id
	// under the following assumptions: (i) Minimal routing is
	// considered, (ii) constant packet size is considered (as the
	// average between the minimum and the maximum packet size).
	double getCommunicationEnergy(int src_id, int dst_id);

	// Shows statistics for the current node
	void showStats(int curr_node, std::ostream& out = std::cout, bool header = false);
};
