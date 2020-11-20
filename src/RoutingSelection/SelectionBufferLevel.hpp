#pragma once
#include "SelectionStrategy.hpp"



class SelectionBufferLevel : public SelectionStrategy
{
public:
	Connection Apply(const Router& router, const Flit& flit, const std::vector<Connection>& directions) const override;
};

