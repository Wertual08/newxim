#pragma once
#include "SelectionStrategy.hpp"



class SelectionRandomKeepSpace : public SelectionStrategy
{
public:
	std::int32_t Apply(Router& router, Flit& flit, const std::vector<std::int32_t>& directions) const override;
};

