#pragma once 
#include <systemc.h>
#include "Relay.hpp"
#include "Stats.hpp"
#include "RoutingSelection/RoutingAlgorithm.hpp"
#include "RoutingSelection/SelectionStrategy.hpp"



class Router : public sc_module
{
	SC_HAS_PROCESS(Router);
private:
	const RoutingAlgorithm* Routing;
	const SelectionStrategy* Selection;

	Router(sc_module_name, const SimulationTimer& timer, std::int32_t id, std::size_t relays, std::int32_t max_buffer_size);

	void Update();

protected:
	std::int32_t start_from_port;			// Port from which to start the reservation cycle

	std::int32_t PerformRoute(const RouteData& route_data);
	void Route(std::int32_t in_port, std::int32_t out_port);

	virtual void TXProcess() = 0;		// The transmitting process
	void RXProcess();					// The receiving process

public:
	std::vector<Relay> Relays;
	Relay& LocalRelay;
	const std::int32_t LocalRelayID;

	// I/O Ports
	sc_in_clk clock;					// The input clock for the router
	sc_in<bool> reset;					// The reset signal for the router

	// Registers
	const std::int32_t LocalID;			// Unique ID
	Stats stats;						// Statistics
	Power power;

	Router(const SimulationTimer& timer, std::int32_t id, std::size_t relays, std::int32_t max_buffer_size);
	void SetRoutingAlgorithm(const RoutingAlgorithm& alg);
	void SetSelectionStrategy(const SelectionStrategy& sel);
};


