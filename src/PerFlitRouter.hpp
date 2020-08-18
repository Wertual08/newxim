#pragma once
#include "Router.hpp"



class PerFlitRouter : public Router
{
protected:
	virtual void TXProcess() override;
	virtual void RXProcess() override;

public:
	PerFlitRouter(const SimulationTimer& timer, std::int32_t id, size_t relays, std::int32_t max_buffer_size);
};