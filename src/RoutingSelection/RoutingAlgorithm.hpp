#pragma once
#include <vector>
#include "../DataStructs.hpp"
#include "../Utils.hpp"



class Router;
class RoutingAlgorithm
{
public:
	virtual std::vector<int32_t> Route(Router& router, const RouteData& routeData) = 0;
};
