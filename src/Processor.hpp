#pragma once
#include <queue>
#include <cstdint>
#include <systemc.h>

#include "Relay.hpp"
#include "GlobalTrafficTable.hpp"
#include "TrafficManager.hpp"
#include "Utils.hpp"



class Processor : public sc_module
{
	SC_HAS_PROCESS(Processor);
private:
	const SimulationTimer Timer;
	std::int32_t TotalFlitsGenerated;
	std::int32_t MaxID;
	const TrafficManager* Traffic;

	size_t packets_in_queue = 0;
	double oldest_packet_time_stamp = 0.0;
	double newest_packet_time_stamp = 0.0;
	Packet current_packet;

	const bool TableBased;
	const double PacketInjectionRate;
	const double ProbabilityOfRetransmission;
	const std::int32_t MinPacketSize;
	const std::int32_t MaxPacketSize;

	Processor(sc_module_name, const SimulationTimer& timer, std::int32_t id, bool table_based,
		double packet_injection_rate, double probability_of_retransmission,
		std::int32_t min_packet_size, std::int32_t max_packet_size, std::int32_t max_id);

	void UpdateCurrentPacket();
	void PushPacket();
	void PopPacket();
	Packet& FrontPacket();
	bool PacketQueueEmpty() const;
	size_t PacketQueueSize() const;

public:
	// I/O Ports
	sc_in_clk clock;		// The input clock for the PE
	sc_in<bool> reset;	// The reset signal for the PE

	Relay relay;

	// Registers
	const std::int32_t local_id;		// Unique identification number
	bool current_level_rx;	// Current level for Alternating Bit Protocol (ABP)
	bool current_level_tx;	// Current level for Alternating Bit Protocol (ABP)
	bool transmittedAtPreviousCycle;	// Used for distributions with memory

	Processor(const SimulationTimer& timer, std::int32_t id, bool table_based,
		double packet_injection_rate, double probability_of_retransmission,
		std::int32_t min_packet_size, std::int32_t max_packet_size, std::int32_t max_id);
	void SetTrafficManager(const TrafficManager& traffic);

	// Functions
	void rxProcess();				// The receiving process
	void txProcess();				// The transmitting process
	bool canShot();	// True when the packet must be shot
	Flit nextFlit();				// Take the next flit of the current packet

	GlobalTrafficTable* traffic_table;	// Reference to the Global traffic Table
	bool never_transmit;				// true if the PE does not transmit any packet 
	//  (valid only for the table based traffic)

	std::int32_t getRandomSize();				// Returns a random size in flits for the packet

	uint32_t getQueueSize() const;

	std::int32_t GetTotalFlitsGenerated() const;
};
