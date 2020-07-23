#pragma once
#include "SelectionStrategy.h"



class SelectionRandomKeepSpace : public SelectionStrategy
{
public:
	int32_t Apply(Router& router, const std::vector<int32_t>& directions, const RouteData& route_data) override;
	void PerCycleProcess(Router& router) override;
};

