#pragma once
#include <vector>
#include "../DataStructs.h"
#include "../Utils.h"



class Router;
class RoutingAlgorithm
{
public:
	virtual std::vector<int32_t> Route(Router& router, const RouteData& routeData) = 0;
};
