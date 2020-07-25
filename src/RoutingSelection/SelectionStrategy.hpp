#pragma once
#include <vector>
#include "../DataStructs.hpp"
#include "../Utils.hpp"



class Router;
class SelectionStrategy
{
public:
	virtual int32_t Apply(Router& router, const std::vector<int32_t>& directions, const RouteData& route_data) = 0;
	virtual void PerCycleProcess(Router& router) = 0;
};
