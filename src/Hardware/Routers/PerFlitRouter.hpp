#pragma once
#include "Router.hpp"



class PerFlitRouter : public Router
{
protected:
	virtual void TXProcess() override;

public:
	PerFlitRouter(const SimulationTimer& timer, std::int32_t id, std::size_t relays);
};