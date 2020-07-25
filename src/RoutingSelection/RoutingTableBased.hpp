#pragma once
#include "RoutingAlgorithm.hpp"
#include "../RoutingTable.hpp"
#include "../Router.hpp"



class RoutingTableBased : public RoutingAlgorithm
{
private:
	const RoutingTable& Table;

public:
	RoutingTableBased(const RoutingTable& table);

	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};
