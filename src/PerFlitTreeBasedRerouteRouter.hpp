#pragma once
#include "Router.hpp"



class PerFlitTreeBasedRerouteRouter : public Router
{
protected:
	virtual void TXProcess() override;

	std::vector<std::vector<std::int32_t>> SubTreeTable;

public:
	PerFlitTreeBasedRerouteRouter(const SimulationTimer& timer, std::int32_t id, std::size_t relays, std::int32_t max_buffer_size);
	void SetupSubTreeTable(const std::vector<std::vector<std::int32_t>>& table);
};