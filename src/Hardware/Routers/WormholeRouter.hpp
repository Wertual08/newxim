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
	WormholeRouter(const SimulationTimer& timer, std::int32_t id, std::size_t relays);
};