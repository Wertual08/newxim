#pragma once
#include "SubnetworkRouter.hpp"



class PerFlitSubnetworkRouter : public SubnetworkRouter
{
protected:
	virtual void TXProcess() override;

public:
	PerFlitSubnetworkRouter(const SimulationTimer& timer, std::int32_t id, std::size_t relays, std::int32_t max_buffer_size);
};