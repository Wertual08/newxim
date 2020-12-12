#pragma once
#include "Router.hpp"
#include "Hardware/ReservationTable.hpp"



class WormholeRouter : public Router
{
private:
	ReservationTable reservation_table;

protected:
	virtual void TXProcess() override;

public:
	using Router::Router;

	virtual const ReservationTable* GetReservationTable() const override { return &reservation_table; }
};