#pragma once
#include "Router.h"
#include "ReservationTable.h"



class WormholeRouter : public Router
{
private:
	ReservationTable reservation_table;	// Switch reservation table

protected:
	virtual void TXProcess() override;
	virtual void RXProcess() override;

public:
	WormholeRouter(sc_module_name mname, int32_t id, 
		size_t relays, int32_t max_buffer_size,
		RoutingAlgorithm& alg, SelectionStrategy& sel);
};