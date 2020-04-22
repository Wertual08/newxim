#pragma once
#include "SelectionStrategy.h"
#include "../Router.h"



class SelectionRandom : public SelectionStrategy 
{
public:
	int Apply(Router& router, const std::vector<int32_t>& directions, const RouteData& route_data) override;
	void PerCycleProcess(Router& router) override;
};
