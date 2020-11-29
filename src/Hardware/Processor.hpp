#pragma once
#include <queue>
#include <cstdint>
#include <systemc.h>

#include "SimulationTimer.hpp"
#include "Relay.hpp"
#include "Configuration/TrafficManagers/TrafficManager.hpp"
#include "Utils.hpp"
#include "ProcessorQueue.hpp"
#include "Metrics/FlitTracer.hpp"



class Processor : public sc_module
{
	SC_HAS_PROCESS(Processor);
private:
	const SimulationTimer Timer;
	std::int32_t MaxID;
	const TrafficManager* Traffic;
	ProcessorQueue Queue;
	Packet& GetQueueFront();

	const std::size_t MinPacketSize;
	const std::size_t MaxPacketSize;

	std::size_t TotalPacketsReceived;
	std::size_t TotalFlitsSent;
	std::size_t TotalFlitsReceived;
	std::size_t TotalActualFlitsSent;
	std::size_t TotalActualFlitsReceived;

	double TotalPacketsDelay;
	double MaxPacketDelay;
	double SimulationMaxTimeFlitInNetwork;
	double SimulationLastTimeFlitReceived;

	FlitTracer* Tracer = nullptr;

	void ReceiveFlit(Flit flit);
	void SendFlit(Flit flit);

	Flit NextFlit();					// Take the next flit of the current packet
	void PopFlit();

	Processor(sc_module_name, const SimulationTimer& timer, std::int32_t id,
		std::int32_t min_packet_size, std::int32_t max_packet_size);

public:
	// I/O Ports
	sc_in_clk clock;		// The input clock for the PE
	sc_in<bool> reset;	// The reset signal for the PE

	Relay relay;

	// Registers
	const std::int32_t local_id;		// Unique identification number
	bool transmittedAtPreviousCycle;	// Used for distributions with memory

	Processor(const SimulationTimer& timer, std::int32_t id,
		std::int32_t min_packet_size, std::int32_t max_packet_size);
	void SetTrafficManager(const TrafficManager& traffic);
	void SetFlitTracer(FlitTracer& tracer);

	// Functions
	void Update();
	void TXProcess();					// The transmitting process
	void RXProcess();					// The receiving process

	std::size_t FlitsSent() const;
	std::size_t FlitsReceived() const;
	std::size_t FlitsProduced() const;
	std::size_t ActualFlitsSent() const;
	std::size_t ActualFlitsReceived() const;

	std::size_t PacketsReceived() const;
	double AverageDelay() const;
	double MaxDelay() const;
	double MaxTimeFlitInNetwork() const;
	double LastReceivedFlitTime() const;
};
