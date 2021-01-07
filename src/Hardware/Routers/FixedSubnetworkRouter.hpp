#pragma once
#include "Hardware/ReservationTable.hpp"
#include "SubnetworkRouter.hpp"



class FixedSubnetworkRouter : public SubnetworkRouter
{
protected:
	virtual void TXProcess() override;

public:
	using SubnetworkRouter::SubnetworkRouter;
};