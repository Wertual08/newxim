#pragma once
#include "Router.hpp"
#include "ReservationTable.hpp"



class WormholeRouter : public Router
{
private:
	ReservationTable reservation_table;

protected:
	virtual void TXProcess() override;

public:
	WormholeRouter(const SimulationTimer& timer, std::int32_t id, size_t relays, std::int32_t max_buffer_size);
};