#pragma once
#include "SelectionStrategy.hpp"



class SelectionKeepSpace : public SelectionStrategy
{
public:
	Connection Apply(const Router& router, const Flit& flit, const std::vector<Connection>& directions) const override;
};

