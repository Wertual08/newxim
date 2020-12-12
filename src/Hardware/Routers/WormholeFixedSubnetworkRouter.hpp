#pragma once
#include "Hardware/ReservationTable.hpp"
#include "SubnetworkRouter.hpp"



class WormholeFixedSubnetworkRouter : public SubnetworkRouter
{
private:
	ReservationTable reservation_table;

protected:
	virtual void TXProcess() override;

public:
	using SubnetworkRouter::SubnetworkRouter;
};