#pragma once
#include "SelectionStrategy.hpp"



class SelectionKeepSpace : public SelectionStrategy
{
public:
	std::int32_t Apply(Router& router, Flit& flit, const std::vector<std::int32_t>& directions) const override;
};

