#pragma once 
#include <systemc.h>
#include "Hardware/SimulationTimer.hpp"
#include "Hardware/Relay.hpp"
#include "Metrics/Stats.hpp"
#include "RoutingSelection/RoutingAlgorithm.hpp"
#include "RoutingSelection/SelectionStrategy.hpp"
#include "Hardware/Connection.hpp"
#include "Hardware/ReservationTable.hpp"
#include "Metrics/FlitTracer.hpp"



class Router : public sc_module
{
	SC_HAS_PROCESS(Router);
private:
	const RoutingAlgorithm* Routing = nullptr;
	const SelectionStrategy* Selection = nullptr;
	FlitTracer* Tracer = nullptr;

	Router(sc_module_name, const SimulationTimer& timer, std::int32_t id, std::size_t relays);

	void Update();

protected:
	std::vector<Relay> Relays;
	std::size_t start_from_port;		// Port from which to start the reservation cycle

	ReservationTable reservation_table;

	Connection FindDestination/*...unknown...*/(const Flit& flit);
	bool Route(std::int32_t in_port, Connection dst);

	virtual void TXProcess();			// The transmitting process
	void RXProcess();					// The receiving process

public:
	Relay& LocalRelay;
	const std::int32_t LocalRelayID;

	// I/O Ports
	sc_in_clk clock;					// The input clock for the router
	sc_in<bool> reset;					// The reset signal for the router

	// Registers
	const std::int32_t LocalID;			// Unique ID
	Stats stats;						// Statistics
	Power power;

	Router(const SimulationTimer& timer, std::int32_t id, std::size_t relays);
	void SetRoutingAlgorithm(const RoutingAlgorithm& alg);
	void SetSelectionStrategy(const SelectionStrategy& sel);
	void SetFlitTracer(FlitTracer& tracer);

	std::size_t Size() const { return Relays.size(); }
	Relay& operator[](std::size_t i) { return Relays[i]; }
	const Relay& operator[](std::size_t i) const { return Relays[i]; }
	Buffer& operator[](Connection d) { return Relays[d.port][d.vc]; }
	const Buffer& operator[](Connection d) const { return Relays[d.port][d.vc]; }

	std::size_t TotalBufferedFlits() const;
	std::size_t DestinationFreeSlots(Connection dst) const;

	const ReservationTable &GetReservationTable() const { return reservation_table; }
};


