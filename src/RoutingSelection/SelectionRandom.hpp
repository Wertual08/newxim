#pragma once
#include "SelectionStrategy.hpp"
#include "Hardware/Routers/Router.hpp"



class SelectionRandom : public SelectionStrategy 
{
public:
	std::int32_t Apply(Router& router, const std::vector<std::int32_t>& directions, const RouteData& route_data) const override;
};
