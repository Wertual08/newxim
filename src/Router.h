/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the router
 */
#pragma once 
#include <systemc.h>
#include "DataStructs.h"
#include "Buffer.h"
#include "Stats.h"
#include "RoutingTable.h"
#include "ReservationTable.h"
#include "Utils.h"
#include "RoutingSelection/RoutingAlgorithm.h"
#include "RoutingSelection/SelectionStrategy.h"



extern unsigned int drained_volume;

class Router : public sc_module
{
	SC_HAS_PROCESS(Router);
private:
	int32_t start_from_port;	// Port from which to start the reservation cycle
	uint32_t routed_flits;

	RoutingAlgorithm& Algorithm;
	SelectionStrategy& Strategy;

	int32_t PerformRoute(const RouteData& route_data);
	void PerCycleProcess();
	void TXProcess();			// The transmitting process
	void RXProcess();			// The receiving process
	void Update();

public:
	struct Relay : sc_module
	{
		sc_in<Flit>					rx_flit;				// The input channels 
		sc_in<bool>					rx_req;					// The requests associated with the input channels
		sc_out<bool>				rx_ack;					// The outgoing ack signals associated with the input channels
		sc_out<bool>				rx_buffer_full_status;

		sc_out<Flit>				tx_flit;				// The output channels
		sc_out<bool>				tx_req;					// The requests associated with the output channels
		sc_in<bool>					tx_ack;					// The outgoing ack signals associated with the output channels
		sc_in<bool>					tx_buffer_full_status;

		sc_out<int> free_slots;
		sc_in<int> free_slots_neighbor;

		Buffer buffer;										// buffer[direction][virtual_channel] 
		bool rx_current_level;								// Current level for Alternating Bit Protocol (ABP)
		bool tx_current_level;								// Current level for Alternating Bit Protocol (ABP)

		Relay(sc_module_name)
		{
		}
	};
	sc_vector<Relay> Relays;

	Relay& LocalRelay;
	const int32_t LocalRelayID;

	ReservationTable reservation_table;		// Switch reservation table

	// I/O Ports
	sc_in_clk clock;			// The input clock for the router
	sc_in<bool> reset;			// The reset signal for the router

	// Registers
	int32_t local_id;			// Unique ID
	Stats stats;				// Statistics
	Power power;

	RoutingTable::Node routing_table;		

	Router(sc_module_name, int32_t id, size_t relays, double warm_up_time, uint32_t max_buffer_size,
		RoutingAlgorithm& alg, SelectionStrategy& sel, RoutingTable& grt);

	uint32_t GetRoutedFlits() const;	// Returns the number of routed flits 
	bool InCongestion() const;
};
