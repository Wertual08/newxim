#pragma once
#include "Router.hpp"



class TreeBasedRerouteRouter : public Router
{
protected:
	virtual void TXProcess() override;

	std::vector<std::int32_t> SubTreeTable;

public:
	TreeBasedRerouteRouter(const SimulationTimer& timer, std::int32_t id, std::size_t relays, std::int32_t max_buffer_size);
	void SetupSubTreeTable(const std::vector<std::int32_t>& table);
};