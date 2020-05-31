#pragma once
#include "RoutingAlgorithm.h"
#include "../RoutingTable.h"
#include "../Router.h"



class RoutingTableBased : public RoutingAlgorithm
{
private:
	const RoutingTable& Table;

public:
	RoutingTableBased(const RoutingTable& table);

	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};
