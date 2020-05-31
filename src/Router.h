#pragma once 
#include <systemc.h>
#include "DataStructs.h"
#include "Buffer.h"
#include "Stats.h"
#include "RoutingSelection/RoutingAlgorithm.h"
#include "RoutingSelection/SelectionStrategy.h"



class Router : public sc_module
{
	SC_HAS_PROCESS(Router);
private:
	void Update();
	RoutingAlgorithm& Algorithm;
	SelectionStrategy& Strategy;

protected:
	int32_t start_from_port;			// Port from which to start the reservation cycle

	int32_t PerformRoute(const RouteData& route_data);
	virtual void TXProcess() = 0;		// The transmitting process
	virtual void RXProcess() = 0;		// The receiving process

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

	// I/O Ports
	sc_in_clk clock;					// The input clock for the router
	sc_in<bool> reset;					// The reset signal for the router

	// Registers
	const int32_t LocalID;				// Unique ID
	Stats stats;						// Statistics
	Power power;

	Router(sc_module_name, int32_t id, size_t relays, int32_t max_buffer_size,
		RoutingAlgorithm& alg, SelectionStrategy& sel);
};


