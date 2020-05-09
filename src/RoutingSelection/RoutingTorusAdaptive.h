#pragma once
#include "RoutingAlgorithm.h"
#include "../Graph.h"



class RoutingTorusAdaptive : public RoutingAlgorithm
{
private:
	const int32_t TorusW, TorusH;
	const Graph& TorusGraph;

	int32_t GetXFromID(int32_t id);
	int32_t GetYFromID(int32_t id);
	int32_t GetIDFromXY(int32_t x, int32_t y);
	int32_t GetLinkTo(int32_t id, int32_t x, int32_t y);

public:
	RoutingTorusAdaptive(int32_t w, int32_t h, const Graph& graph);

	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};
