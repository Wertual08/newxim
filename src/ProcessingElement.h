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

#include "DataStructs.h"
#include "GlobalTrafficTable.h"
#include "Utils.h"



class ProcessingElement : public sc_module
{
	SC_HAS_PROCESS(ProcessingElement);
private:
	int32_t MaxID;

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
	std::queue<Packet> packet_queue;	// Local queue of packets
	bool transmittedAtPreviousCycle;	// Used for distributions with memory

	// Functions
	void rxProcess();				// The receiving process
	void txProcess();				// The transmitting process
	bool canShot(Packet& packet);	// True when the packet must be shot
	Flit nextFlit();				// Take the next flit of the current packet
	Packet trafficTest();			// used for testing traffic
	Packet trafficRandom();			// Random destination distribution
	Packet trafficLocal();			// Random with locality

	GlobalTrafficTable* traffic_table;	// Reference to the Global traffic Table
	bool never_transmit;				// true if the PE does not transmit any packet 
	//  (valid only for the table based traffic)

	int randInt(int min, int max);		// Extracts a random integer number between min and max
	int getRandomSize();				// Returns a random size in flits for the packet
	void setBit(int& x, int w, int v);
	int getBit(int x, int w);

	unsigned int getQueueSize() const;

	// Constructor
	ProcessingElement(sc_module_name, int32_t max_id) : MaxID(max_id)
	{
		SC_METHOD(rxProcess);
		sensitive << reset << clock.pos();

		SC_METHOD(txProcess);
		sensitive << reset << clock.pos();
	}
};
