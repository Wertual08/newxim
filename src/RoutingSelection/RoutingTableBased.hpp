#pragma once
#include "RoutingAlgorithm.hpp"
#include "Configuration/RoutingTable.hpp"
#include "Hardware/Routers/Router.hpp"



class RoutingTableBased : public RoutingAlgorithm
{
private:
	const RoutingTable& Table;

public:
	RoutingTableBased(const RoutingTable& table);

	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};
