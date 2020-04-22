#pragma once
#include "RoutingAlgorithm.h"
#include "../Router.h"



class RoutingTableBased : public RoutingAlgorithm
{
public:
	std::vector<int32_t> Route(Router& router, const RouteData& routeData) override;
};
