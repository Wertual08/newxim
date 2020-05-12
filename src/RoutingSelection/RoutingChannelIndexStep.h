#pragma once
#include "RoutingAlgorithm.h"
#include "../Router.h"



class RoutingChannelIndexStep : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;

public:
	RoutingChannelIndexStep(const Graph& graph);
	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};
