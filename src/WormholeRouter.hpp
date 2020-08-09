#pragma once
#include "Router.hpp"
#include "ReservationTable.hpp"



class WormholeRouter : public Router
{
private:
	ReservationTable reservation_table;	// Switch reservation table

protected:
	virtual void TXProcess() override;
	virtual void RXProcess() override;

public:
	WormholeRouter(sc_module_name mname, const SimulationTimer& timer, int32_t id,
		size_t relays, int32_t max_buffer_size,
		RoutingAlgorithm& alg, SelectionStrategy& sel);
};