/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the processing element
 */
#pragma once
#include <queue>
#include <systemc.h>

#include "DataStructs.hpp"
#include "GlobalTrafficTable.hpp"
#include "TrafficManager.hpp"
#include "Utils.hpp"



class Processor : public sc_module
{
	SC_HAS_PROCESS(Processor);
private:
	const SimulationTimer Timer;
	int32_t TotalFlitsGenerated;
	int32_t MaxID;
	const TrafficManager& Traffic;

	size_t packets_in_queue = 0;
	double oldest_packet_time_stamp = 0.0;
	double newest_packet_time_stamp = 0.0;
	Packet current_packet;

	const bool TableBased;
	const double PacketInjectionRate;
	const double ProbabilityOfRetransmission;
	const int32_t MinPacketSize;
	const int32_t MaxPacketSize;

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

	sc_in<Flit> flit_rx;	// The input channel
	sc_in<bool> req_rx;	// The request associated with the input channel
	sc_out<bool> ack_rx;	// The outgoing ack signal associated with the input channel
	sc_out<bool> buffer_full_status_rx;

	sc_out<Flit> flit_tx;	// The output channel
	sc_out<bool> req_tx;	// The request associated with the output channel
	sc_in<bool> ack_tx;	// The outgoing ack signal associated with the output channel
	sc_in<bool> buffer_full_status_tx;

	sc_in<int> free_slots_neighbor;

	// Registers
	int local_id;		// Unique identification number
	bool current_level_rx;	// Current level for Alternating Bit Protocol (ABP)
	bool current_level_tx;	// Current level for Alternating Bit Protocol (ABP)
	bool transmittedAtPreviousCycle;	// Used for distributions with memory

	// Functions
	void rxProcess();				// The receiving process
	void txProcess();				// The transmitting process
	bool canShot();	// True when the packet must be shot
	Flit nextFlit();				// Take the next flit of the current packet

	GlobalTrafficTable* traffic_table;	// Reference to the Global traffic Table
	bool never_transmit;				// true if the PE does not transmit any packet 
	//  (valid only for the table based traffic)

	int getRandomSize();				// Returns a random size in flits for the packet

	unsigned int getQueueSize() const;

	// Constructor
	Processor(sc_module_name, SimulationTimer timer, bool table_based,
		double packet_injection_rate, double probability_of_retransmission,
		int32_t min_packet_size, int32_t max_packet_size, int32_t max_id, const TrafficManager& traffic) :
		Timer(timer), TableBased(table_based), PacketInjectionRate(packet_injection_rate),
		ProbabilityOfRetransmission(probability_of_retransmission), MinPacketSize(min_packet_size),
		MaxPacketSize(max_packet_size), MaxID(max_id), Traffic(traffic)
	{
		SC_METHOD(rxProcess);
		sensitive << reset << clock.pos();
		SC_METHOD(txProcess);
		sensitive << reset << clock.pos();
	}

	int32_t GetTotalFlitsGenerated() const;
};
