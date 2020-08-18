#pragma once
#include "SelectionStrategy.hpp"



class SelectionBufferLevel : public SelectionStrategy
{
public:
	std::int32_t Apply(Router& router, const std::vector<std::int32_t>& directions, const RouteData& route_data) const override;
};

