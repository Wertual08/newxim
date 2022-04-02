#pragma once 
#include <systemc.h>
#include "Hardware/SimulationTimer.hpp"
#include "Hardware/Relay.hpp"
#include "Metrics/Stats.hpp"
#include "Hardware/Connection.hpp"
#include "Hardware/ReservationTable.hpp"
#include "Metrics/FlitTracer.hpp"



class RoutingAlgorithm;
class SelectionStrategy;
class Router : public sc_module
{
	SC_HAS_PROCESS(Router);
private:
	std::vector<Connection> routing_buffer;

	const RoutingAlgorithm* routing = nullptr;
	const SelectionStrategy* selection = nullptr;
	FlitTracer* tracer = nullptr;

	Router(sc_module_name, const SimulationTimer& timer, std::int32_t id, std::size_t size);

	void Reservation(std::int32_t in_port);
	void Update();

protected:
	std::vector<Relay> relays;
	std::size_t start_from_port;		// Port from which to start the reservation cycle

	std::vector<std::int32_t> update_sequence;

	ReservationTable reservation_table;

	Connection FindDestination/*...unknown...*/(const Flit& flit);
	bool Route(std::int32_t in_port, Connection dst);

	virtual void TXProcess();			// The transmitting process
	void RXProcess();					// The receiving process

public:
	Relay& LocalRelay;
	const std::int32_t LocalRelayId;

	// I/O Ports
	sc_in_clk clock;					// The input clock for the router
	sc_in<bool> reset;					// The reset signal for the router

	// Registers
	const std::int32_t LocalId;			// Unique ID
	Stats stats;						// Statistics

	Router(const SimulationTimer& timer, std::int32_t id, std::size_t size);
	void SetRoutingAlgorithm(const RoutingAlgorithm& alg);
	void SetSelectionStrategy(const SelectionStrategy& sel);
	void SetFlitTracer(FlitTracer& tracer);
	void SetUpdateSequence(const std::vector<std::int32_t> &sequence);

	std::size_t Size() const { return relays.size(); }
	Relay& operator[](std::size_t i) { return relays[i]; }
	const Relay& operator[](std::size_t i) const { return relays[i]; }
	Buffer& operator[](Connection d) { return relays[d.port][d.vc]; }
	const Buffer& operator[](Connection d) const { return relays[d.port][d.vc]; }

	std::size_t TotalBufferedFlits() const;
	std::size_t DestinationFreeSlots(Connection dst) const;
	bool CanSend(Connection dst) const;

	const ReservationTable &GetReservationTable() const { return reservation_table; }
};


