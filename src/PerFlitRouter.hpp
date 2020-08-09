#pragma once
#include "Router.hpp"



class PerFlitRouter : public Router
{
protected:
	virtual void TXProcess() override;
	virtual void RXProcess() override;

public:
	PerFlitRouter(sc_module_name mname, const SimulationTimer& timer, 
		int32_t id, size_t relays, int32_t max_buffer_size,
		RoutingAlgorithm& alg, SelectionStrategy& sel);
};