#pragma once
#include "SelectionStrategy.hpp"
#include "Hardware/Routers/Router.hpp"



class SelectionRandom : public SelectionStrategy 
{
public:
	Connection Apply(const Router& router, const Flit& flit, const std::vector<Connection>& directions) const override;
};
