#pragma once
#include "Hardware/ReservationTable.hpp"
#include "SubnetworkRouter.hpp"



class FitSubnetworkRouter : public SubnetworkRouter
{
protected:
	virtual void TXProcess() override;

public:
	using SubnetworkRouter::SubnetworkRouter;
};