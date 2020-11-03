#pragma once
#include "Hardware/ReservationTable.hpp"
#include "SubnetworkRouter.hpp"



class WormholeSubnetworkRouter : public SubnetworkRouter
{
private:
	ReservationTable reservation_table;

protected:
	virtual void TXProcess() override;

public:
	WormholeSubnetworkRouter(const SimulationTimer& timer, std::int32_t id, std::size_t relays);
};