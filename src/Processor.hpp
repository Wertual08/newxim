#pragma once
#include <queue>
#include <cstdint>
#include <systemc.h>

#include "SimulationTimer.hpp"
#include "Relay.hpp"
#include "TrafficManager.hpp"
#include "Utils.hpp"
#include "ProcessorQueue.hpp"



class Processor : public sc_module
{
	SC_HAS_PROCESS(Processor);
private:
	const SimulationTimer Timer;
	std::int32_t TotalFlitsGenerated;
	std::int32_t MaxID;
	const TrafficManager* Traffic;
	ProcessorQueue Queue;
	Packet& GetQueueFront();

	const std::int32_t MinPacketSize;
	const std::int32_t MaxPacketSize;

	void ReceiveFlit(Flit flit);
	Processor(sc_module_name, const SimulationTimer& timer, std::int32_t id,
		std::int32_t min_packet_size, std::int32_t max_packet_size);

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

	Processor(const SimulationTimer& timer, std::int32_t id,
		std::int32_t min_packet_size, std::int32_t max_packet_size);
	void SetTrafficManager(const TrafficManager& traffic);

	// Functions
	void rxProcess();				// The receiving process
	void txProcess();				// The transmitting process
	Flit nextFlit();				// Take the next flit of the current packet

	std::int32_t getRandomSize();				// Returns a random size in flits for the packet

	std::int32_t GetTotalFlitsGenerated() const;
};
