#pragma once
#include <vector>
#include "../DataStructs.hpp"
#include "../Utils.hpp"



class Router;
class SelectionStrategy
{
public:
	virtual std::int32_t Apply(Router& router, const std::vector<std::int32_t>& directions, const RouteData& route_data) const = 0;
};
